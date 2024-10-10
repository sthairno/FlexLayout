#include <yoga/YGNodeStyle.h>
#include "StylePropertyDefinition.hpp"
#include "TreeContext.hpp"
#include "FlexBoxImpl.hpp"
#include "Config.hpp"

namespace FlexLayout::Internal
{
	using ValueType = Style::StyleValue::Type;

	/// @brief <length>の値を計算する
	static float CalculateCSSLength(Style::StyleValue value, const ComputedTextStyle& style)
	{
		assert(value.type() == ValueType::Length);

		float demicalValue = value.getFloatValueUnchecked();
		switch (value.lengthUnit())
		{
		case LengthUnit::Pixel: return demicalValue;
		case LengthUnit::Ch: return style.zeroGlyphAdvancePx() * demicalValue;
		case LengthUnit::Em: return style.fontSizePx * demicalValue;
		case LengthUnit::Ex: return style.xGlyphHeightPx() * demicalValue;
		case LengthUnit::Ic: return style.cjkWaterGlyphAdvancePx() * demicalValue;
		case LengthUnit::Lh: return style.lineHeightPx() * demicalValue;
		}

		return 0.0;
	}

	// --- Pattern宣言用のヘルパー関数 ---

	template<class Type>
	constexpr static Type PatternEnum() { return Type{}; }

	constexpr static std::vector<std::vector<Style::detail::StyleValueMultiMatchRule>> PatternSingle(Style::detail::StyleValueMultiMatchRule argPattern)
	{
		return { { argPattern } };
	}

	constexpr static std::vector<std::vector<Style::detail::StyleValueMultiMatchRule>> PatternEdge(Style::detail::StyleValueMultiMatchRule argPattern)
	{
		return {
			{ argPattern },
			{ argPattern, argPattern },
			{ argPattern, argPattern, argPattern },
			{ argPattern, argPattern, argPattern, argPattern }
		};
	}

	// --- Yogaの関数をStyleValueを相互運用するための関数 ---

	template<class Enum>
	static auto EnumToYogaEnum(Enum src)
	{
		return Style::detail::style_enum_traits<Enum>::to_yoga[static_cast<std::underlying_type_t<Enum>>(src)];
	}

	static Style::StyleValue YGValueToStyleValue(YGValue src)
	{
		switch (src.unit) {
		case YGUnitUndefined: return Style::StyleValue{ };
		case YGUnitAuto: return Style::StyleValue::Auto();
		case YGUnitPoint: return Style::StyleValue::Length(src.value);
		case YGUnitPercent: return Style::StyleValue::Percentage(src.value);
		}

		return { };
	}

	static bool YogaSetStyleValue(
		YGNodeRef node,
		Style::StyleValue value,
		const ComputedTextStyle& textStyle,
		void(*setter)(YGNodeRef, float),
		void(*percentSetter)(YGNodeRef, float) = nullptr,
		void(*autoSetter)(YGNodeRef) = nullptr)
	{
		assert(setter);

		switch (value.type())
		{
		case ValueType::Length:
			setter(node, CalculateCSSLength(value, textStyle));
			return true;
		case ValueType::Percentage:
			if (percentSetter)
			{
				percentSetter(node, value.getFloatValueUnchecked());
				return true;
			}
			break;
		case ValueType::Auto:
			if (autoSetter)
			{
				autoSetter(node);
				return true;
			}
			break;
		case ValueType::Unspecified:
			setter(node, YGUndefined);
			return true;
		}

		return false;
	}

	template<class Param>
	static bool YogaSetStyleValue(
		YGNodeRef node,
		Param param,
		Style::StyleValue value,
		const ComputedTextStyle& textStyle,
		void(*setter)(YGNodeRef, Param, float),
		void(*percentSetter)(YGNodeRef, Param, float) = nullptr,
		void(*autoSetter)(YGNodeRef, Param) = nullptr)
	{
		assert(setter);

		switch (value.type())
		{
		case ValueType::Length:
			setter(node, param, CalculateCSSLength(value, textStyle));
			return true;
		case ValueType::Percentage:
			if (percentSetter)
			{
				percentSetter(node, param, value.getFloatValueUnchecked());
				return true;
			}
			break;
		case ValueType::Auto:
			if (autoSetter)
			{
				autoSetter(node, param);
				return true;
			}
			break;
		case ValueType::Unspecified:
			setter(node, param, YGUndefined);
			return true;
		}

		return false;
	}

	static bool YogaSetYGValue(
		YGNodeRef node,
		YGValue value,
		void(*setter)(YGNodeRef, float),
		void(*percentSetter)(YGNodeRef, float) = nullptr,
		void(*autoSetter)(YGNodeRef) = nullptr)
	{
		assert(setter);

		switch (value.unit)
		{
		case YGUnitUndefined:
			setter(node, YGUndefined);
			return true;
		case YGUnitAuto:
			if (autoSetter)
			{
				autoSetter(node);
				return true;
			}
			break;
		case YGUnitPoint:
			setter(node, value.value);
			return true;
		case YGUnitPercent:
			if (percentSetter)
			{
				percentSetter(node, value.value);
				return true;
			}
			break;
		}

		return false;
	}

	// --- InstallCallbackの生成を簡略化する補助関数 ---

	template<class Enum, class _YogaEnum>
	static StyleInstallCallback InstallCallback_YogaEnum(void(*setter)(YGNodeRef, _YogaEnum))
	{
		return [setter](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
			{
				if (auto src = input[0].getEnumValue<Enum>())
				{
					setter(impl.yogaNode(), EnumToYogaEnum(*src));
					return true;
				}

				return false;
			};
	}

	static StyleInstallCallback InstallCallback_YogaEdge(
		void(*setter)(YGNodeRef, YGEdge, float),
		void(*percentSetter)(YGNodeRef, YGEdge, float) = nullptr,
		void(*autoSetter)(YGNodeRef, YGEdge) = nullptr)
	{
		return [=](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
			{
				Style::StyleValue t, r, b, l;

				switch (input.size())
				{
				case 1:
					t = r = b = l = input[0];
					break;
				case 2:
					t = b = input[0];
					l = r = input[1];
					break;
				case 3:
					t = input[0];
					r = l = input[1];
					b = input[2];
					break;
				case 4:
					t = input[0];
					r = input[1];
					b = input[2];
					l = input[3];
					break;
				default:
					return false;
				}

				YogaSetStyleValue<YGEdge>(impl.yogaNode(), YGEdgeTop, t, impl.computedTextStyle(), setter, percentSetter, autoSetter);
				YogaSetStyleValue<YGEdge>(impl.yogaNode(), YGEdgeRight, r, impl.computedTextStyle(), setter, percentSetter, autoSetter);
				YogaSetStyleValue<YGEdge>(impl.yogaNode(), YGEdgeBottom, b, impl.computedTextStyle(), setter, percentSetter, autoSetter);
				YogaSetStyleValue<YGEdge>(impl.yogaNode(), YGEdgeLeft, l, impl.computedTextStyle(), setter, percentSetter, autoSetter);

				return false;
			};
	}


	static StyleInstallCallback InstallCallback_YGValue(
		void(*setter)(YGNodeRef, float),
		void(*percentSetter)(YGNodeRef, float) = nullptr,
		void(*autoSetter)(YGNodeRef) = nullptr)
	{
		return [=](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
			{
				if (input.size() != 1)
				{
					return false;
				}

				return YogaSetStyleValue(
					impl.yogaNode(),
					input[0],
					impl.computedTextStyle(),
					setter,
					percentSetter,
					autoSetter
				);
			};
	}

	template<class Param>
	static StyleInstallCallback InstallCallback_YGValue(
		Param param,
		void(*setter)(YGNodeRef, Param, float),
		void(*percentSetter)(YGNodeRef, Param, float) = nullptr,
		void(*autoSetter)(YGNodeRef, Param) = nullptr)
	{
		return [=](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
			{
				if (input.size() != 1)
				{
					return false;
				}

				return YogaSetStyleValue<Param>(
					impl.yogaNode(),
					param,
					input[0],
					impl.computedTextStyle(),
					setter,
					percentSetter,
					autoSetter
				);
			};
	}

	// --- ResetCallbackの生成を簡略化する補助関数 ---

	/// @brief dummyNodeを使ってYogaのEnumデフォルト値を取得し、それを設定するResetCallbackを生成する
	template<class ValueT>
	static StyleResetCallback ResetCallback_YogaDefaultValue(
		ValueT(*getter)(YGNodeConstRef),
		void(*setter)(YGNodeRef, ValueT))
	{
		return [getter, setter](FlexBoxImpl& impl) -> void
			{
				ValueT value = getter(GetConfig().dummyNode());
				setter(impl.yogaNode(), value);
			};
	}

	/// @brief YGUndefinedをサポートしているSetter関数を呼び出すResetCallbackを生成する
	static StyleResetCallback ResetCallback_YogaOptional(void(*setter)(YGNodeRef, float))
	{
		return [setter](FlexBoxImpl& impl) -> void
			{
				setter(impl.yogaNode(), YGUndefined);
			};
	}

	template<class Param>
	static StyleResetCallback ResetCallback_YogaOptional(Param param, void(*setter)(YGNodeRef, Param, float))
	{
		return [setter, param](FlexBoxImpl& impl) -> void
			{
				setter(impl.yogaNode(), param, YGUndefined);
			};
	}

	static StyleResetCallback ResetCallback_YogaEdge(void(*setter)(YGNodeRef, YGEdge, float))
	{
		return [setter](FlexBoxImpl& impl) -> void
			{
				setter(impl.yogaNode(), YGEdgeTop, YGUndefined);
				setter(impl.yogaNode(), YGEdgeRight, YGUndefined);
				setter(impl.yogaNode(), YGEdgeBottom, YGUndefined);
				setter(impl.yogaNode(), YGEdgeLeft, YGUndefined);
			};
	}

	// --- プロパティの宣言 ---

	const static Style::detail::StyleValueMultiMatchRule FlexGrowPattern{ ValueType::Number };
	const static Style::detail::StyleValueMultiMatchRule FlexShrinkPattern{ ValueType::Number };
	const static Style::detail::StyleValueMultiMatchRule FlexBasisPattern{ ValueType::Length, ValueType::Percentage, ValueType::Auto };

	const StylePropertyDefinitionContainer StylePropertyDefinitionList = {
		{
			U"align-content",
			StylePropertyDefinitionDetails {
				.patterns = PatternSingle({ PatternEnum<AlignContent>() }),
				.installCallback = InstallCallback_YogaEnum<AlignContent>(YGNodeStyleSetAlignContent),
				.resetCallback = ResetCallback_YogaDefaultValue(YGNodeStyleGetAlignContent, YGNodeStyleSetAlignContent)
			}
		},
		{
			U"align-items",
			StylePropertyDefinitionDetails {
				.patterns = PatternSingle({ PatternEnum<AlignItems>() }),
				.installCallback = InstallCallback_YogaEnum<AlignItems>(YGNodeStyleSetAlignItems),
				.resetCallback = ResetCallback_YogaDefaultValue(YGNodeStyleGetAlignItems, YGNodeStyleSetAlignItems)
			}
		},
		{
			U"align-self",
			StylePropertyDefinitionDetails {
				.patterns = PatternSingle({ PatternEnum<AlignSelf>() }),
				.installCallback = InstallCallback_YogaEnum<AlignSelf>(YGNodeStyleSetAlignSelf),
				.resetCallback = ResetCallback_YogaDefaultValue(YGNodeStyleGetAlignSelf, YGNodeStyleSetAlignSelf)
			}
		},

		{
			U"aspect-ratio",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ Style::StyleValue::Type::Ratio }),
				.installCallback = [](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
				{
					YGNodeStyleSetAspectRatio(impl.yogaNode(), input[0].getFloatValueUnchecked());
					return true;
				},
				.resetCallback = ResetCallback_YogaOptional(YGNodeStyleSetAspectRatio)
			}
		},
		{
			U"display",
			StylePropertyDefinitionDetails {
				.patterns = PatternSingle({ PatternEnum<Display>() }),
				.installCallback = InstallCallback_YogaEnum<Display>(YGNodeStyleSetDisplay),
				.resetCallback = ResetCallback_YogaDefaultValue(YGNodeStyleGetDisplay, YGNodeStyleSetDisplay)
			}
		},
		{
			// https://developer.mozilla.org/ja/docs/Web/CSS/flex
			U"flex",
			StylePropertyDefinitionDetails {
				.patterns = {
					{ FlexGrowPattern | FlexBasisPattern },
					{ FlexGrowPattern, FlexShrinkPattern | FlexBasisPattern },
					{ FlexGrowPattern, FlexShrinkPattern, FlexBasisPattern }
				},
				.installCallback = [](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
				{
					float grow, shrink;
					Style::StyleValue basis;

					// 引数の数で分岐

					if (input.size() == 1)
					{
						// 0: <flex-glow>|<flex-basis>

						switch (input[0].type())
						{
						case ValueType::Number:
							grow = input[0].getFloatValueUnchecked();
							shrink = 1;
							basis = Style::StyleValue::Length(0);
							break;
						default:
							grow = 1;
							shrink = 1;
							basis = input[0];
							break;
						}
					}
					else if (input.size() == 2)
					{
						// 0: <flex-glow>
						// 1: <flex-shrink>|<flex-basis>

						grow = input[0].getFloatValueUnchecked();

						switch (input[1].type())
						{
						case ValueType::Number:
							shrink = input[1].getFloatValueUnchecked();
							basis = Style::StyleValue::Length(0);
							break;
						default:
							shrink = 1;
							basis = input[1];
							break;
						}
					}
					else if (input.size() == 3)
					{
						// 0: <flex-glow>
						// 1: <flex-shrink>
						// 2: <flex-basis>

						grow = input[0].getFloatValueUnchecked();
						shrink = input[1].getFloatValueUnchecked();
						basis = input[2];
					}
					else
					{
						return false;
					}

					// 設定を反映

					YGNodeStyleSetFlexGrow(impl.yogaNode(), grow);
					YGNodeStyleSetFlexShrink(impl.yogaNode(), shrink);
					YogaSetStyleValue(
						impl.yogaNode(),
						basis,
						impl.computedTextStyle(),
						YGNodeStyleSetFlexBasis,
						YGNodeStyleSetFlexBasisPercent,
						YGNodeStyleSetFlexBasisAuto
					);

					return true;
				},
				.resetCallback = [](FlexBoxImpl& impl) -> void
				{
					YGNodeStyleSetFlexGrow(impl.yogaNode(), YGUndefined);
					YGNodeStyleSetFlexShrink(impl.yogaNode(), YGUndefined);
					YGNodeStyleSetFlexBasis(impl.yogaNode(), YGUndefined);
				},
				.maybeAffectTo = { U"flex-grow", U"flex-shrink", U"flex-basis" }
			}
		},
		{
			U"flex-basis",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle(FlexBasisPattern),
				.installCallback = InstallCallback_YGValue(YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent),
				.resetCallback = ResetCallback_YogaOptional(YGNodeStyleSetFlexBasis)
			}
		},
		{
			U"flex-grow",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle(FlexGrowPattern),
				.installCallback = [](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
				{
					YGNodeStyleSetFlexGrow(impl.yogaNode(), input[0].getFloatValueUnchecked());
					return true;
				},
				.resetCallback = ResetCallback_YogaOptional(YGNodeStyleSetFlexGrow)
			}
		},
		{
			U"flex-shrink",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle(FlexShrinkPattern),
				.installCallback = [](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
				{
					YGNodeStyleSetFlexShrink(impl.yogaNode(), input[0].getFloatValueUnchecked());
					return true;
				},
				.resetCallback = ResetCallback_YogaOptional(YGNodeStyleSetFlexShrink)
			}
		},
		{
			U"flex-direction",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ PatternEnum<FlexDirection>() }),
				.installCallback = InstallCallback_YogaEnum<FlexDirection>(YGNodeStyleSetFlexDirection),
				.resetCallback = ResetCallback_YogaDefaultValue(YGNodeStyleGetFlexDirection, YGNodeStyleSetFlexDirection)
			}
		},
		{
			U"flex-flow",
			StylePropertyDefinitionDetails{
				.patterns = {
					{ { PatternEnum<FlexDirection>(), PatternEnum<FlexWrap>() } },
					{ { PatternEnum<FlexDirection>() }, { PatternEnum<FlexWrap>() } }
				},
				.installCallback = [](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
				{
					switch (input.size())
					{
					case 1:
						if (auto direction = input[0].getEnumValue<FlexDirection>())
						{
							YGNodeStyleSetFlexDirection(impl.yogaNode(), EnumToYogaEnum(*direction));
							return true;
						}
						else if (auto wrap = input[0].getEnumValue<FlexWrap>())
						{
							YGNodeStyleSetFlexWrap(impl.yogaNode(), EnumToYogaEnum(*wrap));
							return true;
						}
						break;
					case 2:
						YGNodeStyleSetFlexDirection(
							impl.yogaNode(),
							EnumToYogaEnum(input[0].getEnumValueUnchecked<FlexDirection>())
						);
						YGNodeStyleSetFlexWrap(
							impl.yogaNode(),
							EnumToYogaEnum(input[1].getEnumValueUnchecked<FlexWrap>())
						);
						return true;
					}

					return false;
				},
				.resetCallback = [](FlexBoxImpl& impl) -> void
				{
					auto dummyNode = GetConfig().dummyNode();
					YGNodeStyleSetFlexDirection(impl.yogaNode(), YGNodeStyleGetFlexDirection(dummyNode));
					YGNodeStyleSetFlexWrap(impl.yogaNode(), YGNodeStyleGetFlexWrap(dummyNode));
				},
				.maybeAffectTo = { U"flex-direction", U"flex-wrap" }
			}
		},
		{
			U"flex-wrap",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ PatternEnum<FlexWrap>() }),
				.installCallback = InstallCallback_YogaEnum<FlexWrap>(YGNodeStyleSetFlexWrap),
				.resetCallback = ResetCallback_YogaDefaultValue(YGNodeStyleGetFlexWrap, YGNodeStyleSetFlexWrap)
			}
		},
		{
			U"gap",
			StylePropertyDefinitionDetails{
				.patterns = {
					{ { ValueType::Length, ValueType::Percentage } },
					{ { ValueType::Length, ValueType::Percentage }, { ValueType::Length, ValueType::Percentage } }
				},
				.installCallback = [](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
				{
					Style::StyleValue row, column;

					switch (input.size())
					{
					case 1:
						row = input[0];
						column = input[0];
						break;
					case 2:
						row = input[0];
						column = input[1];
						break;
					default:
						return false;
					}

					YogaSetStyleValue(
						impl.yogaNode(),
						YGGutterRow,
						row,
						impl.computedTextStyle(),
						YGNodeStyleSetGap,
						YGNodeStyleSetGapPercent
					);
					YogaSetStyleValue(
						impl.yogaNode(),
						YGGutterColumn,
						column,
						impl.computedTextStyle(),
						YGNodeStyleSetGap,
						YGNodeStyleSetGapPercent
					);

					return false;
				},
				.resetCallback = [](FlexBoxImpl& impl) -> void
				{
					YGNodeStyleSetGap(impl.yogaNode(), YGGutterRow, YGUndefined);
					YGNodeStyleSetGap(impl.yogaNode(), YGGutterColumn, YGUndefined);
				},
				.maybeAffectTo = { U"row-gap", U"column-gap" }
			}
		},
		{
			U"row-gap",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGGutterRow, YGNodeStyleSetGap, YGNodeStyleSetGapPercent),
				.resetCallback = ResetCallback_YogaOptional(YGGutterRow, YGNodeStyleSetGap)
			}
		},
		{
			U"column-gap",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGGutterColumn, YGNodeStyleSetGap, YGNodeStyleSetGapPercent),
				.resetCallback = ResetCallback_YogaOptional(YGGutterColumn, YGNodeStyleSetGap)
			}
		},
		{
			U"position",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ PatternEnum<Position>() }),
				.installCallback = InstallCallback_YogaEnum<Position>(YGNodeStyleSetPositionType),
				.resetCallback = ResetCallback_YogaDefaultValue(YGNodeStyleGetPositionType, YGNodeStyleSetPositionType)
			}
		},
		{
			U"top",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGEdgeTop, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeTop, YGNodeStyleSetPosition)
			}
		},
		{
			U"right",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGEdgeRight, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeRight, YGNodeStyleSetPosition)
			}
		},
		{
			U"bottom",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGEdgeBottom, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeBottom, YGNodeStyleSetPosition)
			}
		},
		{
			U"left",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGEdgeLeft, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeLeft, YGNodeStyleSetPosition)
			}
		},
		{
			U"justify-content",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ PatternEnum<JustifyContent>() }),
				.installCallback = InstallCallback_YogaEnum<JustifyContent>(YGNodeStyleSetJustifyContent),
				.resetCallback = ResetCallback_YogaDefaultValue(YGNodeStyleGetJustifyContent, YGNodeStyleSetJustifyContent)
			}
		},
		{
			U"direction",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ PatternEnum<Direction>() }),
				.installCallback = InstallCallback_YogaEnum<Direction>(YGNodeStyleSetDirection),
				.resetCallback = ResetCallback_YogaDefaultValue(YGNodeStyleGetDirection, YGNodeStyleSetDirection)
			}
		},
		{
			U"margin",
			StylePropertyDefinitionDetails{
				.patterns = PatternEdge({ ValueType::Length, ValueType::Percentage, ValueType::Auto }),
				.installCallback = InstallCallback_YogaEdge(YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto),
				.resetCallback = ResetCallback_YogaEdge(YGNodeStyleSetMargin),
				.maybeAffectTo = { U"margin-top", U"margin-right", U"margin-bottom", U"margin-left" }
			}
		},
		{
			U"margin-top",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage, ValueType::Auto }),
				.installCallback = InstallCallback_YGValue(YGEdgeTop, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeTop, YGNodeStyleSetMargin)
			}
		},
		{
			U"margin-right",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage, ValueType::Auto }),
				.installCallback = InstallCallback_YGValue(YGEdgeRight, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeRight, YGNodeStyleSetMargin)
			}
		},
		{
			U"margin-bottom",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage, ValueType::Auto }),
				.installCallback = InstallCallback_YGValue(YGEdgeBottom, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeBottom, YGNodeStyleSetMargin)
			}
		},
		{
			U"margin-left",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage, ValueType::Auto }),
				.installCallback = InstallCallback_YGValue(YGEdgeLeft, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeLeft, YGNodeStyleSetMargin)
			}
		},
		{
			U"padding",
			StylePropertyDefinitionDetails{
				.patterns = PatternEdge({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YogaEdge(YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent),
				.resetCallback = ResetCallback_YogaEdge(YGNodeStyleSetPadding),
				.maybeAffectTo = { U"padding-top", U"padding-right", U"padding-bottom", U"padding-left" }
			}
		},
		{
			U"padding-top",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage}),
				.installCallback = InstallCallback_YGValue(YGEdgeTop, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeTop, YGNodeStyleSetPadding)
			}
		},
		{
			U"padding-right",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGEdgeRight, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeRight, YGNodeStyleSetPadding)
			}
		},
		{
			U"padding-bottom",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGEdgeBottom, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeBottom, YGNodeStyleSetPadding)
			}
		},
		{
			U"padding-left",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGEdgeLeft, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeLeft, YGNodeStyleSetPadding)
			}
		},
		{
			U"border-width",
			StylePropertyDefinitionDetails{
				.patterns = PatternEdge({ ValueType::Length }),
				.installCallback = InstallCallback_YogaEdge(YGNodeStyleSetBorder),
				.resetCallback = ResetCallback_YogaEdge(YGNodeStyleSetBorder),
				.maybeAffectTo = { U"border-top-width", U"border-right-width", U"border-bottom-width", U"border-left-width" }
			}
		},
		{
			U"border-top-width",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length }),
				.installCallback = InstallCallback_YGValue(YGEdgeTop, YGNodeStyleSetBorder),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeTop, YGNodeStyleSetBorder)
			}
		},
		{
			U"border-right-width",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length }),
				.installCallback = InstallCallback_YGValue(YGEdgeRight, YGNodeStyleSetBorder),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeRight, YGNodeStyleSetBorder)
			}
		},
		{
			U"border-bottom-width",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length }),
				.installCallback = InstallCallback_YGValue(YGEdgeBottom, YGNodeStyleSetBorder),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeBottom, YGNodeStyleSetBorder)
			}
		},
		{
			U"border-left-width",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length }),
				.installCallback = InstallCallback_YGValue(YGEdgeLeft, YGNodeStyleSetBorder),
				.resetCallback = ResetCallback_YogaOptional(YGEdgeLeft, YGNodeStyleSetBorder)
			}
		},
		{
			U"width",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage, ValueType::Auto }),
				.installCallback = InstallCallback_YGValue(YGNodeStyleSetWidth, YGNodeStyleSetWidthPercent, YGNodeStyleSetWidthAuto),
				.resetCallback = ResetCallback_YogaOptional(YGNodeStyleSetWidth)
			}
		},
		{
			U"height",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage, ValueType::Auto }),
				.installCallback = InstallCallback_YGValue(YGNodeStyleSetHeight, YGNodeStyleSetHeightPercent, YGNodeStyleSetHeightAuto),
				.resetCallback = ResetCallback_YogaOptional(YGNodeStyleSetHeight)
			}
		},
		{
			U"min-width",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGNodeStyleSetMinWidth, YGNodeStyleSetMinWidthPercent),
				.resetCallback = ResetCallback_YogaOptional(YGNodeStyleSetMinWidth)
			}
		},
		{
			U"min-height",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGNodeStyleSetMinHeight, YGNodeStyleSetMinHeightPercent),
				.resetCallback = ResetCallback_YogaOptional(YGNodeStyleSetMinHeight)
			}
		},
		{
			U"max-width",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGNodeStyleSetMaxWidth, YGNodeStyleSetMaxWidthPercent),
				.resetCallback = ResetCallback_YogaOptional(YGNodeStyleSetMaxWidth)
			}
		},
		{
			U"max-height",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = InstallCallback_YGValue(YGNodeStyleSetMaxHeight, YGNodeStyleSetMaxHeightPercent),
				.resetCallback = ResetCallback_YogaOptional(YGNodeStyleSetMaxHeight)
			}
		},
		{
			U"font-size",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Length, ValueType::Percentage }),
				.installCallback = [](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
				{
					auto& textStyle = impl.computedTextStyle();

					// 親要素からサイズを継承、もしemなどが使用された場合は親要素からの相対値として計算

					if (auto parent = impl.parent())
					{
						textStyle.fontSizePx = parent->computedTextStyle().fontSizePx;
					}
					else
					{
						textStyle.fontSizePx = GetConfig().defaultTextStyle().fontSizePx;
					}

					// サイズ計算

					auto value = input[0];
					switch (value.type())
					{
					case ValueType::Length:
						textStyle.fontSizePx = CalculateCSSLength(input[0], textStyle);
						break;
					case ValueType::Percentage:
						textStyle.fontSizePx = textStyle.fontSizePx * value.getFloatValueUnchecked() / 100.0f;
						break;
					}

					return true;
				},
				.resetCallback = [](FlexBoxImpl& impl) -> void
				{
					if (auto parent = impl.parent())
					{
						impl.computedTextStyle().fontSizePx = parent->computedTextStyle().fontSizePx;
					}
					else
					{
						impl.computedTextStyle().fontSizePx = GetConfig().defaultTextStyle().fontSizePx;
					}
				}
			}
		},
		{
			U"line-height",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ ValueType::Number }),
				.installCallback = [](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
				{
					impl.computedTextStyle().lineHeightMul = input[0].getFloatValueUnchecked();

					return true;
				},
				.resetCallback = [](FlexBoxImpl& impl) -> void
				{
					if (auto parent = impl.parent())
					{
						impl.computedTextStyle().lineHeightMul = parent->computedTextStyle().lineHeightMul;
					}
					else
					{
						impl.computedTextStyle().lineHeightMul = GetConfig().defaultTextStyle().lineHeightMul;
					}
				}
			}
		},
		{
			U"text-align",
			StylePropertyDefinitionDetails{
				.patterns = PatternSingle({ PatternEnum<TextAlign>() }),
				.installCallback = [](FlexBoxImpl& impl, std::span<const Style::StyleValue> input) -> bool
				{
					impl.computedTextStyle().textAlign = input[0].getEnumValueUnchecked<TextAlign>();

					return true;
				},
				.resetCallback = [](FlexBoxImpl& impl) -> void
				{
					if (auto parent = impl.parent())
					{
						impl.computedTextStyle().textAlign = parent->computedTextStyle().textAlign;
					}
					else
					{
						impl.computedTextStyle().textAlign = GetConfig().defaultTextStyle().textAlign;
					}
				}
			}
		}
	};
}
