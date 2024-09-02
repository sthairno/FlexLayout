#include "StylePropertyDefinition.hpp"
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
		return [setter](FlexBoxImpl& impl, std::span<const Style::StyleValue> values) -> bool
			{
				if (values.size() != 1)
				{
					return false;
				}

				if (auto src = values[0].getEnumValue<Enum>())
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

	const std::unordered_map<StringView, StylePropertyDefinition> StyleProperties = {
		{
			U"align-content",
			StylePropertyDefinition {
				.patterns = Single({ Enum<AlignContent>() }),
				.applyCallback = YogaEnumApplyCallback<AlignContent>(YGNodeStyleSetAlignContent),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetAlignContent, YGNodeStyleSetAlignContent)
			}
		},
		{
			U"align-items",
			StylePropertyDefinition {
				.patterns = Single({ Enum<AlignItems>() }),
				.applyCallback = YogaEnumApplyCallback<AlignItems>(YGNodeStyleSetAlignItems),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetAlignItems, YGNodeStyleSetAlignItems)
			}
		},
		{
			U"align-self",
			StylePropertyDefinition {
				.patterns = Single({ Enum<AlignSelf>() }),
				.applyCallback = YogaEnumApplyCallback<AlignSelf>(YGNodeStyleSetAlignSelf),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetAlignSelf, YGNodeStyleSetAlignSelf)
			}
		},

		{
			U"aspect-ratio",
			StylePropertyDefinition{
				.patterns = Single({ Style::StyleValue::Type::Ratio }),
			}
		},
		{
			U"display",
			StylePropertyDefinition {
				.patterns = Single({ Enum<Display>() }),
				.applyCallback = YogaEnumApplyCallback<Display>(YGNodeStyleSetDisplay),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetDisplay, YGNodeStyleSetDisplay)
			}
		},
		{
			U"flex",
			StylePropertyDefinition{
				.patterns = {
					{ FlexGrowPattern | FlexBasisPattern },
					{ FlexGrowPattern, FlexShrinkPattern },
					{ FlexGrowPattern, FlexShrinkPattern, FlexBasisPattern }
				},
			}
		},
		{
			U"flex-basis",
			StylePropertyDefinition{
				.patterns = Single(FlexBasisPattern),
			}
		},
		{
			U"flex-grow",
			StylePropertyDefinition{
				.patterns = Single(FlexGrowPattern),
			}
		},
		{
			U"flex-shrink",
			StylePropertyDefinition{
				.patterns = Single(FlexShrinkPattern),
			}
		},
		{
			U"flex-direction",
			StylePropertyDefinition{
				.patterns = Single({ Enum<FlexDirection>() }),
				.applyCallback = YogaEnumApplyCallback<FlexDirection>(YGNodeStyleSetFlexDirection),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetFlexDirection, YGNodeStyleSetFlexDirection)
			}
		},
		{
			U"flex-flow",
			StylePropertyDefinition{
				.patterns = {
					{ { Enum<FlexDirection>(), Enum<FlexWrap>() } },
					{ { Enum<FlexDirection>() }, { Enum<FlexWrap>() } }
				}
			}
		},
		{
			U"flex-wrap",
			StylePropertyDefinition{
				.patterns = Single({ Enum<FlexWrap>() }),
				.applyCallback = YogaEnumApplyCallback<FlexWrap>(YGNodeStyleSetFlexWrap),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetFlexWrap, YGNodeStyleSetFlexWrap)
			}
		},
		{
			U"gap",
			StylePropertyDefinition{
				.patterns = {
					{ { ValueType::Length, ValueType::Percentage } },
					{ { ValueType::Length, ValueType::Percentage }, { ValueType::Length, ValueType::Percentage } }
				}
			}
		},
		{
			U"row-gap",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
		},
		{
			U"column-gap",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
		},
		{
			U"position",
			StylePropertyDefinition{
				.patterns = Single({ Enum<Position>() }),
				.applyCallback = YogaEnumApplyCallback<Position>(YGNodeStyleSetPositionType),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetPositionType, YGNodeStyleSetPositionType)
			}
		},
		{
			U"top",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"right",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"bottom",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"left",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"justify-content",
			StylePropertyDefinition{
				.patterns = Single({ Enum<JustifyContent>() }),
				.applyCallback = YogaEnumApplyCallback<JustifyContent>(YGNodeStyleSetJustifyContent),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetJustifyContent, YGNodeStyleSetJustifyContent)
			}
		},
		{
			U"direction",
			StylePropertyDefinition{
				.patterns = Single({ Enum<Direction>() }),
				.applyCallback = YogaEnumApplyCallback<Direction>(YGNodeStyleSetDirection),
				.resetCallback = YogaEnumResetCallback(YGNodeStyleGetDirection, YGNodeStyleSetDirection)
			}
		},
		{
			U"margin",
			StylePropertyDefinition{
				.patterns = Edge({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"margin-top",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"margin-right",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"margin-bottom",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"margin-left",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"padding",
			StylePropertyDefinition{
				.patterns = Edge({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"padding-top",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"padding-right",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"padding-bottom",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"padding-left",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"border-width",
			StylePropertyDefinition{
				.patterns = Edge({ ValueType::Length })
			}
		},
		{
			U"border-top-width",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length })
			}
		},
		{
			U"border-right-width",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length })
			}
		},
		{
			U"border-bottom-width",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length })
			}
		},
		{
			U"border-left-width",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length })
			}
		},
		{
			U"width",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"height",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage, ValueType::Auto })
			}
		},
		{
			U"min-width",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
		},
		{
			U"min-height",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
		},
		{
			U"max-width",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
			},
		{
			U"max-height",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
			},
		{
			U"font-size",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Length, ValueType::Percentage })
			}
		},
		{
			U"line-height",
			StylePropertyDefinition{
				.patterns = Single({ ValueType::Number, ValueType::Length, ValueType::Percentage })
			}
		},
		{
			U"text-align",
			StylePropertyDefinition{
				.patterns = Single({ Enum<TextAlign>() })
			}
		}
	};
}
