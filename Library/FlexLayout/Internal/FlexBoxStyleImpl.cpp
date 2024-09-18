#include "FlexBoxImpl.hpp"
#include <Siv3D/Indexed.hpp>
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

	void FlexBoxImpl::setInlineCssText(const StringView cssText)
	{
		clearStyles(StylePropertyGroup::Inline);

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

			setStyle(StylePropertyGroup::Inline, propertyName, std::array<Style::ValueInputVariant, 1>{ propertyValue });
		}
	}

	String FlexBoxImpl::getInlineCssText() const
	{
		String tmp;

		for (const auto& entry : m_styles.group(StylePropertyGroup::Inline))
		{
			if (not entry.removed())
			{
				tmp += U"{}: {};"_fmt(entry.name(), entry.value.join(U" ", U"", U""));
			}
		}

		return tmp;
	}

	Array<Style::StyleValue> FlexBoxImpl::getStyle(StylePropertyGroup group, const StringView styleName) const
	{
		if (auto entry = m_styles.find(group, styleName))
		{
			return entry->value;
		}

		return { };
	}

	bool FlexBoxImpl::setStyle(StylePropertyGroup group, const StringView styleName, const std::span<const Style::StyleValue> values)
	{
		if (values.empty() ||
			std::all_of(values.begin(), values.end(), [](auto& v){ return v.type() == Style::StyleValue::Type::Unspecified; }))
		{
			return removeStyle(group, styleName);
		}

		auto entry = m_styles.get(group, styleName, true);
		if (not entry)
		{
			return false;
		}

		const auto& patterns = entry->patterns();

		// いずれかのパターンに合致するか検証
		auto patternItr = patterns.begin();
		for (; patternItr != patterns.end(); patternItr++)
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
		if (patternItr == patterns.end())
		{
			return false;
		}

		// スタイルを更新
		entry->value = Array<Style::StyleValue>(values.begin(), values.end());

		scheduleStyleApplication();

		return true;
	}

	bool FlexBoxImpl::setStyle(StylePropertyGroup group, const StringView styleName, std::span<const Style::ValueInputVariant> inputs)
	{
		if (inputs.empty())
		{
			return removeStyle(group, styleName);
		}

		auto entry = m_styles.get(group, styleName, true);
		if (not entry)
		{
			return false;
		}

		const auto& definition = entry->definition();

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
		entry->value = std::move(parsedValues);
		scheduleStyleApplication();

		return true;
	}

	bool FlexBoxImpl::removeStyle(StylePropertyGroup group, const StringView styleName)
	{
		auto entry = m_styles.find(group, styleName);

		if (not entry || entry->removed())
		{
			return false;
		}

		entry->clear();

		scheduleStyleApplication();

		return false;
	}

	void FlexBoxImpl::clearStyles(Optional<StylePropertyGroup> group)
	{
		bool modified = false;

		const auto removeAll = [&](StylePropertyTable::group_span_type g)
		{
			for (auto& entry : g)
			{
				if (not entry.removed())
				{
					entry.clear();
					modified = true;
				}
			}
		};

		if (group)
		{
			removeAll(m_styles.group(*group));
		}
		else
		{
			for (auto& g : m_styles)
			{
				removeAll(g);
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

	void FlexBoxImpl::setFont(const Font& font, const StringView fontId)
	{
		if (font == m_font.font)
		{
			return;
		}

		m_font = _FontProperty{
			.font = font,
			.id = font ? String{ fontId } : U""
		};

		scheduleStyleApplication();
	}

	void FlexBoxImpl::setFont(const StringView fontId)
	{
		if (fontId.empty())
		{
			setFont({ }, U"");
		}
		else
		{
			setFont(m_context->loadFont(fontId), fontId);
		}
	}

	void FlexBoxImpl::applyStylesImpl()
	{
		const static auto ApplyProperty = [](FlexBoxImpl& self, const StyleProperty* prop)
			{
				if (not prop)
				{
					return;
				}

				if (prop->removed())
				{
					prop->reset(self);
				}
				else
				{
					prop->install(self);
				}
			};

		m_isStyleApplicationScheduled = false;

		// font,font-size,line-height,text-alignを事前に計算
		// (emなど、フォントに関連するサイズ計算に必要)

		const static size_t lineHeightHash = StyleProperty::Hash(U"line-height");
		const static size_t fontSizeHash = StyleProperty::Hash(U"font-size");
		const static size_t textAlignHash = StyleProperty::Hash(U"text-align");

		m_computedTextStyle.font = m_font.font
			? m_font.font // 設定値
			: m_parent ? m_parent->m_computedTextStyle.font : m_context->defaultTextStyle().font; // デフォルト値

		auto lineHeightProp = m_styles.find(lineHeightHash);
		ApplyProperty(*this, lineHeightProp);

		auto fontSizeProp = m_styles.find(fontSizeHash);
		ApplyProperty(*this, fontSizeProp);

		auto textAlignProp = m_styles.find(textAlignHash);
		ApplyProperty(*this, textAlignProp);

		// その他のスタイル
		
		// TODO: 優先順位の昇順に適用
		//for (const auto& group : m_styles)
		//{
		//	for (auto& prop : group)
		//	{
		//		if (prop.keyHash() == lineHeightHash ||
		//			prop.keyHash() == fontSizeHash ||
		//			prop.keyHash() == textAlignHash)
		//		{
		//			continue;
		//		}

		//		if (not prop.removed())
		//		{
		//			prop.install(*this);
		//		}
		//	}
		//}

		// 子要素にも再帰

		for (const auto& child : m_children)
		{
			child->applyStylesImpl();
		}
	}
}
