#include "StyleDefinition.hpp"
#include "TreeContext.hpp"
#include <yoga/YGNodeStyle.h>

namespace FlexLayout::Internal
{
	using ValueType = Style::StyleValue::Type;

	template<class Type>
	constexpr static Type Enum() { return Type{}; }

	constexpr static std::vector<std::vector<Style::detail::StyleValueMultiMatchRule>> Single(Style::detail::StyleValueMultiMatchRule argPattern)
	{
		return { { argPattern } };
	}

	constexpr static std::vector<std::vector<Style::detail::StyleValueMultiMatchRule>> Edge(Style::detail::StyleValueMultiMatchRule argPattern)
	{
		return {
			{ argPattern },
			{ argPattern, argPattern },
			{ argPattern, argPattern, argPattern },
			{ argPattern, argPattern, argPattern, argPattern }
		};
	}

	template<class Enum, class YogaSetter>
	static auto YogaEnumApplyCallback(YogaSetter&& setter)
	{
		return [setter](FlexBoxImpl& impl, Style::StyleValue value) -> bool
			{
				if (auto src = value.getEnumValue<Enum>())
				{
					auto dst = Style::detail::style_enum_traits<Enum>::to_yoga[static_cast<std::underlying_type_t<Enum>>(*src)];
					setter(impl.yogaNode(), dst);
					return true;
				}

				return false;
			};
	}

	template<class YogaGetter, class YogaSetter>
	static auto YogaEnumResetCallback(YogaGetter&& getter, YogaSetter&& setter)
	{
		return [getter, setter](FlexBoxImpl& impl) -> bool
			{
				auto value = getter(impl.context()->dummyNode());
				setter(impl.yogaNode(), value);
				return true;
			};
	}

	const static Style::detail::StyleValueMultiMatchRule FlexGrowPattern{ ValueType::Number };
	const static Style::detail::StyleValueMultiMatchRule FlexShrinkPattern{ ValueType::Number };
	const static Style::detail::StyleValueMultiMatchRule FlexBasisPattern{ ValueType::Length, ValueType::Percentage, ValueType::Auto };

	const std::unordered_map<StringView, StyleDefinition> StyleDefinitionList = {
		{
			U"align-content",
			StyleDefinition {
				.patterns = Single({ Enum<AlignContent>() }),
				.applyCallback = YogaEnumApplyCallback<AlignContent>(YGNodeStyleSetAlignContent),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetAlignContent, YGNodeStyleSetAlignContent)
			}
		},
		{
			U"align-items",
			StyleDefinition {
				.patterns = Single({ Enum<AlignItems>() }),
				.applyCallback = YogaEnumApplyCallback<AlignItems>(YGNodeStyleSetAlignItems),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetAlignItems, YGNodeStyleSetAlignItems)
			}
		},
		{
			U"align-self",
			StyleDefinition {
				.patterns = Single({ Enum<AlignSelf>() }),
				.applyCallback = YogaEnumApplyCallback<AlignSelf>(YGNodeStyleSetAlignSelf),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetAlignSelf, YGNodeStyleSetAlignSelf)
			}
		},
		{
			U"aspect-ratio",
			StyleDefinition{
				.patterns = Single({ Style::StyleValue::Type::Ratio }),
			}
		},
		{
			U"display",
			StyleDefinition {
				.patterns = Single({ Enum<Display>() }),
				.applyCallback = YogaEnumApplyCallback<Display>(YGNodeStyleSetDisplay),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetDisplay, YGNodeStyleSetDisplay)
			}
		},
		{
			U"flex",
			StyleDefinition{
				.patterns = {
					{ FlexGrowPattern | FlexBasisPattern },
					{ FlexGrowPattern, FlexShrinkPattern },
					{ FlexGrowPattern, FlexShrinkPattern, FlexBasisPattern }
				},
			}
		},
		{
			U"flex-basis",
			StyleDefinition{
				.patterns = Single(FlexBasisPattern),
			}
		},
		{
			U"flex-grow",
			StyleDefinition{
				.patterns = Single(FlexGrowPattern),
			}
		},
		{
			U"flex-shrink",
			StyleDefinition{
				.patterns = Single(FlexShrinkPattern),
			}
		},
		{
			U"flex-direction",
			StyleDefinition{
				.patterns = Single({ Enum<FlexDirection>() }),
				.applyCallback = YogaEnumApplyCallback<FlexDirection>(YGNodeStyleSetFlexDirection),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetFlexDirection, YGNodeStyleSetFlexDirection)
			}
		},
		{
			U"flex-flow",
			StyleDefinition{
				.patterns = {
					{ { Enum<FlexDirection>(), Enum<FlexWrap>() } },
					{ { Enum<FlexDirection>() }, { Enum<FlexWrap>() } }
				}
			}
		},
		{
			U"flex-wrap",
			StyleDefinition{
				.patterns = Single({ Enum<FlexWrap>() }),
				.applyCallback = YogaEnumApplyCallback<FlexWrap>(YGNodeStyleSetFlexWrap),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetFlexWrap, YGNodeStyleSetFlexWrap)
			}
		},
		{
			U"gap",
			StyleDefinition{
				.patterns = {
					{ { ValueType::Length, ValueType::Percentage } },
					{ { ValueType::Length, ValueType::Percentage }, { ValueType::Length, ValueType::Percentage } }
				}
			}
		},
		{
			U"row-gap",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
		},
		{
			U"column-gap",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
		},
		{
			U"position",
			StyleDefinition{
				.patterns = Single({ Enum<Position>() }),
				.applyCallback = YogaEnumApplyCallback<Position>(YGNodeStyleSetPositionType),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetPositionType, YGNodeStyleSetPositionType)
			}
		},
		{
			U"top",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"right",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"bottom",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"left",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"justify-content",
			StyleDefinition{
				.patterns = Single({ Enum<JustifyContent>() }),
				.applyCallback = YogaEnumApplyCallback<JustifyContent>(YGNodeStyleSetJustifyContent),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetJustifyContent, YGNodeStyleSetJustifyContent)
			}
		},
		{
			U"direction",
			StyleDefinition{
				.patterns = Single({ Enum<Direction>() }),
				.applyCallback = YogaEnumApplyCallback<Direction>(YGNodeStyleSetDirection),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetDirection, YGNodeStyleSetDirection)
			}
		},
		{
			U"margin",
			StyleDefinition{
				.patterns = Edge({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"margin-top",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"margin-right",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"margin-bottom",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"margin-left",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"padding",
			StyleDefinition{
				.patterns = Edge({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"padding-top",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"padding-right",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"padding-bottom",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"padding-left",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"border-width",
			StyleDefinition{
				.patterns = Edge({ ValueType::Length })
			}
		},
		{
			U"border-top-width",
			StyleDefinition{
				.patterns = Single({ ValueType::Length })
			}
		},
		{
			U"border-right-width",
			StyleDefinition{
				.patterns = Single({ ValueType::Length })
			}
		},
		{
			U"border-bottom-width",
			StyleDefinition{
				.patterns = Single({ ValueType::Length })
			}
		},
		{
			U"border-left-width",
			StyleDefinition{
				.patterns = Single({ ValueType::Length })
			}
		},
		{
			U"width",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"height",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"min-width",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
		},
		{
			U"min-height",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
		},
		{
			U"max-width",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
			},
		{
			U"max-height",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
			},
		{
			U"font-size",
			StyleDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
		},
		{
			U"line-height",
			StyleDefinition{
				.patterns = Single({ ValueType::Number, ValueType::Length, ValueType::Percentage })
			}
		},
		{
			U"text-align",
			StyleDefinition{
				.patterns = Single({ Enum<TextAlign>() })
			}
		}
	};
}
