#include "FlexBoxImpl.hpp"
#include "TreeContext.hpp"
#include "../Style/StyleValueParser.hpp"

// FlexBoxImplのスタイルに関する実装

namespace FlexLayout::Internal
{
	void FlexBoxImpl::ApplyStyles(TreeContext& context)
	{
		if (context.m_styleApplicationWaitlist.empty())
		{
			return;
		}

		// 浅い順にソートしなおす
		using _QueueValue = std::pair<size_t, std::shared_ptr<FlexBoxImpl>>;
		using _QueueComp = decltype([](const _QueueValue& a, const _QueueValue& b) { return a.first > b.first; });
		using _QueueType = std::priority_queue<_QueueValue, Array<_QueueValue>, _QueueComp>;
		_QueueType queue;
		for (const auto& weakptr : context.m_styleApplicationWaitlist)
		{
			if (auto item = weakptr.lock())
			{
				queue.push({ item->getDepth(), item });
			}
		}
		context.m_styleApplicationWaitlist.clear();

		// 適用
		while (not queue.empty())
		{
			auto& [depth, item] = queue.top();
			if (item->m_isStyleApplicationScheduled)
			{
				item->applyStylesImpl();
			}
			queue.pop();
		}
	}

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
			endIdx++;

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
		return getStyleProperty(styleName).value;
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

		auto definitionItr = StyleProperties.find(styleName);
		if (definitionItr == StyleProperties.end())
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
		auto& entry = getStyleProperty(styleName);
		entry.value = Array<Style::StyleValue>(values.begin(), values.end());
		entry.removed = false;

		scheduleStyleApplication();

		return true;
	}

	bool FlexBoxImpl::setStyle(const StringView styleName, std::span<const Style::ValueInputVariant> inputs)
	{
		if (inputs.empty())
		{
			return removeStyle(styleName);
		}

		auto definitionItr = StyleProperties.find(styleName);
		if (definitionItr == StyleProperties.end())
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

				if (beginIdx < endIdx)
				{
					tmpInputs.push_back(text.substr(beginIdx, endIdx - beginIdx));
				}

				endIdx++;
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
		auto& entry = getStyleProperty(styleName);
		entry.value = std::move(parsedValues);
		entry.removed = false;
		scheduleStyleApplication();

		return true;
	}

	bool FlexBoxImpl::removeStyle(const StringView styleName)
	{
		auto itr = m_styles.find(styleName);

		if (itr == m_styles.end() || itr->second.removed)
		{
			return false;
		}

		auto& prop = itr->second;

		prop.value.clear();
		prop.removed = true;
		scheduleStyleApplication();

		return false;
	}

	void FlexBoxImpl::clearStyles()
	{
		bool modified = false;
		for (auto& [_, prop] : m_styles)
		{
			if (not prop.removed)
			{
				prop.value.clear();
				prop.removed = true;
				modified = true;
			}
		}

		if (modified)
		{
			scheduleStyleApplication();
		}
	}

	void FlexBoxImpl::scheduleStyleApplication()
	{
		if (m_isStyleApplicationScheduled)
		{
			return;
		}

		// 待機リストに追加
		m_context->m_styleApplicationWaitlist.push_back(weak_from_this());
		m_isStyleApplicationScheduled = true;
	}

	FlexBoxImpl::_StyleProperty& FlexBoxImpl::getStyleProperty(const StringView styleName)
	{
		auto result = m_styles.try_emplace(styleName, _StyleProperty{
			.definition = StyleProperties.at(styleName)
		});
		return result.first->second;
	}

	FlexBoxImpl::_StyleProperty* FlexBoxImpl::tryGetStyleProperty(const StringView styleName)
	{
		auto itr = m_styles.find(styleName);
		return itr != m_styles.end() ? &itr->second : nullptr;
	}

	void FlexBoxImpl::applyStylesImpl()
	{
		const static auto ApplyProperty = [](FlexBoxImpl& self, _StyleProperty& prop)
		{
			if (prop.removed)
			{
				prop.definition.resetCallback(self);
			}
			else
			{
				prop.definition.installCallback(self, prop.value);
			}
		};

		m_isStyleApplicationScheduled = false;

		// font,font-size,line-heightを事前に計算
		// (emなど、フォントに関連するサイズ計算に必要)

		m_computedTextStyle.font = m_parent
			? m_parent->m_computedTextStyle.font
			: m_context->defaultTextStyle().font;

		auto lineHeightProp = getStyleProperty(U"line-height");
		ApplyProperty(*this, lineHeightProp);

		auto fontSizeProp = getStyleProperty(U"font-size");
		ApplyProperty(*this, fontSizeProp);

		// その他のスタイルを適用

		for (auto& [name, prop] : m_styles)
		{
			if (&prop == &fontSizeProp || &prop == &lineHeightProp)
			{
				continue;
			}

			ApplyProperty(*this, prop);
		}

		// 子要素にも再帰

		for (const auto& child : m_children)
		{
			child->applyStylesImpl();
		}
	}
}
