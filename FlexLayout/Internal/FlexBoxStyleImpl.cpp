#include "FlexBoxImpl.hpp"
#include "TreeContext.hpp"
#include "StyleDefinition.hpp"
#include "../Style/StyleValueParser.hpp"

namespace FlexLayout::Internal
{
	void FlexBoxImpl::setCssText(const StringView cssText)
	{
		size_t beginIdx = 0;
		size_t endIdx = 0;
		while (endIdx < cssText.length())
		{
			beginIdx = endIdx;
			endIdx = cssText.indexOf(U';', beginIdx);
			if (endIdx == String::npos)
			{
				endIdx = cssText.length();
			}

			const StringView propertyText = cssText.substr(beginIdx, endIdx - beginIdx);

			const auto colonPos = propertyText.indexOf(U':');
			if (colonPos == String::npos)
			{
				continue;
			}

			String propertyName{ propertyText.substr(0, colonPos) };
			String propertyValue{ propertyText.substr(colonPos + 1) };

			propertyName.trim();
			propertyValue.trim();

			if (propertyName.isEmpty() || propertyValue.isEmpty())
			{
				continue;
			}

			setStyle(propertyName, std::array<Style::ValueInputVariant, 1>{ propertyValue });
		}
	}

	String FlexBoxImpl::getCssText() const
	{
		String tmp;

		for (const auto& [key, value] : m_styles)
		{
			tmp += U"{}: {}"_fmt(key, value.value.join(U" ", U"", U""));
		}

		return tmp;
	}

	const Array<Style::StyleValue>& FlexBoxImpl::getStyle(const StringView styleName)
	{
		return m_styles[styleName].value;
	}

	Array<Style::StyleValue> FlexBoxImpl::getStyle(const StringView styleName) const
	{
		if (auto itr = m_styles.find(styleName); itr != m_styles.end())
		{
			return itr->second.value;
		}

		return { };
	}

	bool FlexBoxImpl::setStyle(const StringView styleName, const std::span<const Style::StyleValue> values)
	{
		if (values.empty() ||
			std::all_of(values.begin(), values.end(), [](auto& v){ return v.type() == Style::StyleValue::Type::Unspecified; }))
		{
			return removeStyle(styleName);
		}

		auto definitionItr = StyleDefinitionList.find(styleName);
		if (definitionItr == StyleDefinitionList.end())
		{
			return false;
		}

		const auto& definition = definitionItr->second;

		// いずれかのパターンに合致するか検証
		auto patternItr = definition.patterns.begin();
		for (; patternItr != definition.patterns.end(); patternItr++)
		{
			auto& pattern = *patternItr;

			if (pattern.size() != values.size())
			{
				continue;
			}

			bool success = true;
			for (auto [idx, value] : Indexed(values))
			{
				if (not pattern[idx].match(value))
				{
					success = false;
					break;
				}
			}

			if (success)
			{
				break;
			}
		}

		// 検証に失敗した場合
		if (patternItr == definition.patterns.end())
		{
			return false;
		}

		// スタイルを作成 or 更新
		auto& entry = m_styles[styleName];
		entry.value = Array<Style::StyleValue>(values.begin(), values.end());
		if (entry.removed)
		{
			entry.event = _StyleValueEntry::Event::Added;
			entry.removed = false;
		}
		else if(entry.event != _StyleValueEntry::Event::Added)
		{
			entry.event = _StyleValueEntry::Event::Modified;
		}

		scheduleStyleApplication();

		return true;
	}

	bool FlexBoxImpl::setStyle(const StringView styleName, std::span<const Style::ValueInputVariant> inputs)
	{
		if (inputs.empty())
		{
			return removeStyle(styleName);
		}

		auto definitionItr = StyleDefinitionList.find(styleName);
		if (definitionItr == StyleDefinitionList.end())
		{
			return false;
		}

		const auto& definition = definitionItr->second;

		// 引数が文字列1つの場合、配列として処理して参照を切り替える
		Array<Style::ValueInputVariant> tmpInputs;
		if (inputs.size() == 1 && std::holds_alternative<const StringView>(inputs[0]))
		{
			const auto text = std::get<const StringView>(inputs[0]);

			size_t beginIdx = 0;
			size_t endIdx = 0;
			while (endIdx < text.length())
			{
				beginIdx = endIdx;
				endIdx = text.indexOfAny(U" \t", beginIdx);
				if (endIdx == String::npos)
				{
					endIdx = text.length();
				}

				tmpInputs.push_back(text.substr(beginIdx, endIdx - beginIdx));
			}
			
			inputs = std::span{ tmpInputs.begin(), tmpInputs.end() };
		}

		// 入力の各要素をStyleValueへ読み込み
		Array<Style::StyleValue> parsedValues(Arg::reserve = inputs.size());
		auto patternItr = definition.patterns.begin();
		for (; patternItr != definition.patterns.end(); patternItr++)
		{
			auto& pattern = *patternItr;
			parsedValues.clear();

			if (pattern.size() != inputs.size())
			{
				continue;
			}

			bool success = true;
			for (auto [idx, input] : Indexed(inputs))
			{
				auto value = Style::ParseValue(input, pattern[idx]);

				if (not value)
				{
					success = false;
					break;
				}

				parsedValues.push_back(value);
			}

			if (success)
			{
				break;
			}
		}

		// 読み込みに失敗した場合
		if (patternItr == definition.patterns.end())
		{
			return false;
		}

		// スタイルを作成 or 更新
		auto& entry = m_styles[styleName];
		entry.value = std::move(parsedValues);
		if (entry.removed)
		{
			entry.event = _StyleValueEntry::Event::Added;
			entry.removed = false;
		}
		else if (entry.event != _StyleValueEntry::Event::Added)
		{
			entry.event = _StyleValueEntry::Event::Modified;
		}

		scheduleStyleApplication();

		return true;
	}

	bool FlexBoxImpl::removeStyle(const StringView styleName)
	{
		auto& entry = m_styles[styleName];

		if (entry.removed)
		{
			return false;
		}

		entry.value.clear();
		switch (entry.event)
		{
		case _StyleValueEntry::Event::Added: // 相殺
			entry.event = _StyleValueEntry::Event::None;
			break;
		default:
			entry.event = _StyleValueEntry::Event::Removed;
			scheduleStyleApplication();
			break;
		}

		return false;
	}

	void FlexBoxImpl::clearStyles()
	{
		for (auto& [_, entry] : m_styles)
		{
			entry.value.clear();
			switch (entry.event)
			{
			case _StyleValueEntry::Event::Added: // 相殺
				entry.event = _StyleValueEntry::Event::None;
				break;
			default:
				entry.event = _StyleValueEntry::Event::Removed;
				break;
			}
		}
		scheduleStyleApplication();
	}

	void FlexBoxImpl::scheduleStyleApplication()
	{
		if (m_isStyleApplicationScheduled)
		{
			return;
		}

		// 親要素がスケジュール済みであるかを確認する
		for (FlexBoxImpl* item = this; item; item = item->parent())
		{
			if (item->m_isStyleApplicationScheduled)
			{
				return;
			}
		}

		// 待機リストに追加 (深さ順にソートされる)
		m_context->m_styleApplicationWaitlist.insert({
			getDepth(), weak_from_this()
		});
	}

	void FlexBoxImpl::applyStylesRecursive()
	{
		m_isStyleApplicationScheduled = false;

		// スタイルを適用
		for (auto& [name, entry] : m_styles)
		{
			// TODO
			
		}

		// 子要素に再帰
		for (const auto& child : m_children)
		{
			child->applyStylesRecursive();
		}
	}
}
