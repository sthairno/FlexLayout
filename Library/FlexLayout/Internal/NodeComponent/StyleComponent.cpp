#include "StyleComponent.hpp"
#include "../FlexBoxNode.hpp"
#include <Siv3D/Indexed.hpp>
#include "../Config.hpp"
#include "../Style/StyleValueParser.hpp"
#include "../TreeContext.hpp"

namespace FlexLayout::Internal::Component
{
	StyleComponent::StyleComponent(FlexBoxNode& node)
		: m_node(node)
	{ }

	void StyleComponent::copy(const StyleComponent& source)
	{
		copyStyles(source);
		copyFont(source);
	}

	void StyleComponent::setInlineCssText(const StringView cssText)
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

	String StyleComponent::getInlineCssText() const
	{
		String tmp;

		for (const auto& entry : m_properties.group(StylePropertyGroup::Inline))
		{
			if (not entry.removed())
			{
				tmp += U"{}: {};"_fmt(entry.definition().name(), entry.value().join(U" ", U"", U""));
			}
		}

		return tmp;
	}

	Array<Style::StyleValue> StyleComponent::getStyle(StylePropertyGroup group, const StringView styleName) const
	{
		if (auto entry = m_properties.find(group, styleName))
		{
			return entry->value();
		}

		return { };
	}

	bool StyleComponent::setStyle(StylePropertyGroup group, const StringView styleName, const std::span<const Style::StyleValue> values)
	{
		if (values.empty() ||
			std::all_of(values.begin(), values.end(), [](auto& v) { return v.type() == Style::StyleValue::Type::Unspecified; }))
		{
			return removeStyle(group, styleName);
		}

		auto entry = m_properties.get(group, styleName, true);
		if (not entry)
		{
			return false;
		}

		const auto& patterns = entry->definition().patterns();

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
		entry->setValue({ values.begin(), values.end() });
		scheduleStyleApplication();

		return true;
	}

	bool StyleComponent::setStyle(StylePropertyGroup group, const StringView styleName, std::span<const Style::ValueInputVariant> inputs)
	{
		if (inputs.empty())
		{
			return removeStyle(group, styleName);
		}

		auto entry = m_properties.get(group, styleName, true);
		if (not entry)
		{
			return false;
		}

		const auto definition = entry->definition();

		// 引数が文字列1つの場合、配列として処理して参照を切り替える
		Array<Style::ValueInputVariant> tmpInputs;
		if (inputs.size() == 1 && std::holds_alternative<StringView>(inputs[0]))
		{
			const auto text = std::get<StringView>(inputs[0]);

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
		auto patternItr = definition.patterns().begin();
		for (; patternItr != definition.patterns().end(); patternItr++)
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
				auto value = ParseValue(input, pattern[idx]);

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
		if (patternItr == definition.patterns().end())
		{
			return false;
		}

		// スタイルを作成 or 更新
		entry->setValue(std::move(parsedValues));
		scheduleStyleApplication();

		return true;
	}

	bool StyleComponent::removeStyle(StylePropertyGroup group, const StringView styleName)
	{
		auto entry = m_properties.find(group, styleName);

		if (not entry || entry->removed())
		{
			return false;
		}

		entry->unsetValue();

		scheduleStyleApplication();

		return false;
	}

	void StyleComponent::clearStyles(Optional<StylePropertyGroup> group)
	{
		bool modified = false;

		const auto removeAll = [&](StylePropertyTable::group_span_type g)
			{
				for (auto& entry : g)
				{
					if (not entry.removed())
					{
						entry.unsetValue();
						modified = true;
					}
				}
			};

		if (group)
		{
			removeAll(m_properties.group(*group));
		}
		else
		{
			for (auto& g : m_properties)
			{
				removeAll(g);
			}
		}

		if (modified)
		{
			scheduleStyleApplication();
		}
	}

	void StyleComponent::copyStyles(StylePropertyGroup group, const StyleComponent& source)
	{
		clearStyles(group);
		for (const auto& prop : source.m_properties.group(group))
		{
			if (prop.removed())
			{
				continue;
			}

			setStyle(group, prop.definition().name(), prop.value());
		}
	}

	void StyleComponent::copyStyles(const StyleComponent& source)
	{
		for (std::underlying_type_t<StylePropertyGroup> groupId = 0; groupId < source.m_properties.size(); groupId++)
		{
			auto group = static_cast<StylePropertyGroup>(groupId);

			copyStyles(group, source);
		}
	}

	void StyleComponent::scheduleStyleApplication()
	{
		if (m_propertyApplicationScheduled)
		{
			return;
		}

		// 待機リストに追加
		m_node.context().getContext<Context::StyleContext>()
			.queueStyleApplication(m_node.shared_from_this());
		m_propertyApplicationScheduled = true;
	}

	void StyleComponent::setFont(const Font& font, const StringView fontId)
	{
		if (font == m_fontProperty.font)
		{
			return;
		}

		m_fontProperty = _FontProperty{
			.font = font,
			.id = font ? String{ fontId } : U""
		};

		scheduleStyleApplication();
	}

	void StyleComponent::setFont(const StringView fontId)
	{
		if (fontId.empty())
		{
			setFont({ }, U"");
		}
		else
		{
			setFont(GetConfig().loadFont(fontId), fontId);
		}
	}

	void StyleComponent::copyFont(const StyleComponent& source)
	{
		setFont(source.m_fontProperty.font, source.m_fontProperty.id);
	}

	void StyleComponent::applyProperties(detail::PropertyApplicationState& state)
	{
		m_propertyApplicationScheduled = false;

		// font,font-size,line-height,text-alignを事前に計算
		// (emなど、フォントに関連するサイズ計算に必要)

		const static size_t lineHeightHash = StyleProperty::Hash(U"line-height");
		const static size_t fontSizeHash = StyleProperty::Hash(U"font-size");
		const static size_t textAlignHash = StyleProperty::Hash(U"text-align");

		constexpr static auto installTextProperty = [](FlexBoxNode& node, StyleProperty* prop) -> void
			{
				if (not prop)
				{
					return;
				}

				prop->clearEvent();

				if (not prop->removed())
				{
					prop->installTo(node);
				}
			};

		ComputedTextStyle prevStyle = m_computedTextStyle;

		m_computedTextStyle = m_node.parent()
			? m_node.parent()->getComponent<StyleComponent>().computedTextStyle()
			: GetConfig().defaultTextStyle();

		if (m_fontProperty.font)
		{
			m_computedTextStyle.font = m_fontProperty.font;
		}

		auto lineHeightProp = m_properties.find(lineHeightHash);
		installTextProperty(m_node, lineHeightProp);

		auto fontSizeProp = m_properties.find(fontSizeHash);
		installTextProperty(m_node, fontSizeProp);

		auto textAlignProp = m_properties.find(textAlignHash);
		installTextProperty(m_node, textAlignProp);

		const bool isInheritedPropertyUpdated = prevStyle != m_computedTextStyle;

		// その他のスタイル

		for (auto& group : m_properties)
		{
			for (auto& prop : group)
			{
				if (&prop == lineHeightProp || &prop == fontSizeProp || &prop == textAlignProp)
				{
					continue;
				}

				auto& propState = state.propertyStates.try_emplace(
					prop.keyHash(),
					detail::InstalledPropertyState{ prop.definition() }
				).first->second;

				if (not prop.removed())
				{
					prop.installTo(m_node);

					propState.propertyRef = &prop;
					propState.priority = state.priorityCounter++;
				}
				else if (prop.event() == StyleProperty::Event::Removed)
				{
					prop.definition().resetCallback(m_node);

					// リセットで影響を受ける、インストール済みのプロパティを再インストール
					auto& affectedKeys = propState.definition.maybeAffectTo();
					if (not affectedKeys.empty())
					{
						std::vector<std::pair<size_t, StyleProperty*>> affectedProperties;

						// 大体2~4個程度なので計算コストは無視できる
						for (const auto& key : affectedKeys)
						{
							const auto hash = StyleProperty::Hash(key);
							if (auto itr = state.propertyStates.find(hash);
								itr != state.propertyStates.end() &&
								itr->second.propertyRef)
							{
								affectedProperties.push_back({
									itr->second.priority,
									itr->second.propertyRef
								});
							}
						}

						std::sort(
							affectedProperties.begin(),
							affectedProperties.end(),
							[](const auto& a, const auto& b) {
								return a.first < b.first;
							}
						);

						for (auto [_, p] : affectedProperties)
						{
							p->installTo(m_node);
						}
					}
				}

				prop.clearEvent();
			}
		}

		if (isInheritedPropertyUpdated)
		{
			for (const auto& child : m_node.children())
			{
				detail::PropertyApplicationState childState;
				child->getComponent<StyleComponent>()
					.applyProperties(childState);
			}
		}
	}
}
