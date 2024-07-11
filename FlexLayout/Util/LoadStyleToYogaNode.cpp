#include "LoadStyleToYogaNode.hpp"
#include "../detail/FlexBoxImpl.hpp"

using namespace std::placeholders;

namespace FlexLayout::Util
{
	template<
		typename ValueT,
		typename SetterFuncT,
		std::enable_if_t<
			std::is_invocable_v<SetterFuncT, YGNodeRef, ValueT>
		>* = nullptr>
	static bool SetYogaEnumProperty(
		detail::FlexBoxImpl& b,
		const StringView value,
		const HashTable<String, ValueT>&valueMapper,
		SetterFuncT&& setter)
	{
		if (const auto it = valueMapper.find(value); it != valueMapper.end())
		{
			setter(b.yogaNode(), it->second);
			return true;
		}

		return false;
	}

	using SetterFunc = void (*)(YGNodeRef);

	using FloatSetterFunc = void (*)(YGNodeRef, float);

	template<typename ArgT>
	using SetterFuncWithArg = void (*)(YGNodeRef, ArgT);

	template<typename ArgT>
	using FloatSetterFuncWithArg = void (*)(YGNodeRef, ArgT, float);

	static bool SetYogaFloatProperty(
		detail::FlexBoxImpl& b,
		const StringView value,
		FloatSetterFunc setter)
	{
		if (auto v = ParseFloatOpt<float>(value))
		{
			setter(b.yogaNode(), *v);
			return true;
		}

		return false;
	}

	static bool SetYogaScaleProperty(
		detail::FlexBoxImpl& b,
		const StringView value,
		FloatSetterFunc setterPixels)
	{
		if (value.ends_with(U"px"_sv))
		{
			if (auto v = ParseFloatOpt<float>(value.substr(0, value.size() - 2)))
			{
				setterPixels(b.yogaNode(), *v);
				return true;
			}
		}
		else
		{
			if (auto v = ParseFloatOpt<float>(value))
			{
				setterPixels(b.yogaNode(), *v);
				return true;
			}
		}

		return false;
	}

	static bool SetYogaScaleProperty(
		detail::FlexBoxImpl& b,
		const StringView value,
		FloatSetterFunc setterPixels,
		FloatSetterFunc setterPercentage)
	{
		if (value.ends_with(U"%"_sv))
		{
			if (auto v = ParseFloatOpt<float>(value.substr(0, value.size() - 1)))
			{
				setterPercentage(b.yogaNode(), *v);
				return true;
			}
		}

		return SetYogaScaleProperty(b, value, setterPixels);
	}

	static bool SetYogaScaleProperty(
		detail::FlexBoxImpl& b,
		const StringView value,
		FloatSetterFunc setterPixels,
		FloatSetterFunc setterPercentage,
		SetterFunc setterAuto)
	{
		if (value == U"auto")
		{
			setterAuto(b.yogaNode());
			return true;
		}

		return SetYogaScaleProperty(b, value, setterPixels, setterPercentage);
	}

	template<typename ArgT>
	static bool SetYogaScaleProperty(
		detail::FlexBoxImpl& b,
		const StringView value,
		ArgT param,
		FloatSetterFuncWithArg<ArgT> setterPixels)
	{
		if (value.ends_with(U"px"_sv))
		{
			if (auto v = ParseFloatOpt<float>(value.substr(0, value.size() - 2)))
			{
				setterPixels(b.yogaNode(), param, *v);
				return true;
			}
		}
		else
		{
			if (auto v = ParseFloatOpt<float>(value))
			{
				setterPixels(b.yogaNode(), param, *v);
				return true;
			}
		}

		return false;
	}

	template<typename ArgT>
	static bool SetYogaScaleProperty(
		detail::FlexBoxImpl& b,
		const StringView value,
		ArgT arg,
		FloatSetterFuncWithArg<ArgT> setterPixels,
		FloatSetterFuncWithArg<ArgT> setterPercentage)
	{
		if (value.ends_with(U"%"_sv))
		{
			if (auto v = ParseFloatOpt<float>(value.substr(0, value.size() - 1)))
			{
				setterPercentage(b.yogaNode(), arg, *v);
				return true;
			}
		}

		return SetYogaScaleProperty<ArgT>(b, value, arg, setterPixels);
	}

	template<typename ArgT>
	static bool SetYogaScaleProperty(
		detail::FlexBoxImpl& b,
		const StringView value,
		ArgT arg,
		FloatSetterFuncWithArg<ArgT> setterPixels,
		FloatSetterFuncWithArg<ArgT> setterPercentage,
		SetterFuncWithArg<ArgT> setterAuto)
	{
		if (value == U"auto")
		{
			setterAuto(b.yogaNode(), arg);
			return true;
		}

		return SetYogaScaleProperty<ArgT>(b, value, arg, setterPixels, setterPercentage);
	}

	static bool SetFlex(detail::FlexBoxImpl& b, const StringView value)
	{
		auto params = String{ value }.split(U' ');
		auto node = b.yogaNode();

		switch (params.size())
		{
		case 1:
			if (params[0] == U"initial")
			{
				YGNodeStyleSetFlexGrow(node, 0);
				YGNodeStyleSetFlexShrink(node, 1);
				YGNodeStyleSetFlexBasisAuto(node);
				return true;
			}
			else if (params[0] == U"auto")
			{
				YGNodeStyleSetFlexGrow(node, 1);
				YGNodeStyleSetFlexShrink(node, 1);
				YGNodeStyleSetFlexBasisAuto(node);
				return true;
			}
			else if (params[0] == U"none")
			{
				YGNodeStyleSetFlexGrow(node, 0);
				YGNodeStyleSetFlexShrink(node, 0);
				YGNodeStyleSetFlexBasisAuto(node);
				return true;
			}
			else if (SetYogaFloatProperty(b, params[0], YGNodeStyleSetFlexGrow))
			{
				YGNodeStyleSetFlexShrink(node, 1);
				YGNodeStyleSetFlexBasis(node, 0);
				return true;
			}
			else if (SetYogaScaleProperty(b, params[0], YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto))
			{
				YGNodeStyleSetFlexGrow(node, 1);
				YGNodeStyleSetFlexShrink(node, 1);
				return true;
			}
		case 2:
			SetYogaFloatProperty(b, params[0], YGNodeStyleSetFlexGrow);
			if (SetYogaFloatProperty(b, params[0], YGNodeStyleSetFlexShrink))
			{
				YGNodeStyleSetFlexBasis(node, 0);
				return true;
			}
			else if (SetYogaScaleProperty(b, params[1], YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto))
			{
				YGNodeStyleSetFlexShrink(node, 1);
				return true;
			}
		case 3:
			SetYogaFloatProperty(b, params[0], YGNodeStyleSetFlexGrow);
			SetYogaFloatProperty(b, params[1], YGNodeStyleSetFlexShrink);
			SetYogaScaleProperty(b, params[2], YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto);
			return true;
		}

		return false;
	}

	static bool SetGap(detail::FlexBoxImpl& b, const StringView value)
	{
		bool success = false;
		auto params = String{ value }.split(U' ');

		switch (params.size())
		{
		case 1:
			return SetYogaScaleProperty(
				b, params[0],
				YGGutterAll,
				YGNodeStyleSetGap,
				YGNodeStyleSetGapPercent
			);
		case 2:
			success = SetYogaScaleProperty(
				b, params[0],
				YGGutterRow,
				YGNodeStyleSetGap,
				YGNodeStyleSetGapPercent
			);
			success &= SetYogaScaleProperty(
				b, params[1],
				YGGutterColumn,
				YGNodeStyleSetGap,
				YGNodeStyleSetGapPercent
			);
			return success;
		}

		return false;
	}

	static bool SetMargin(detail::FlexBoxImpl& b, const StringView value)
	{
		bool success = false;
		auto params = String{ value }.split(U' ');

		switch (params.size())
		{
		case 1:
			return SetYogaScaleProperty(
				b, params[0],
				YGEdgeAll,
				YGNodeStyleSetMargin,
				YGNodeStyleSetMarginPercent,
				YGNodeStyleSetMarginAuto
			);
			break;
		case 2:
			success = true;
			for (auto [i, edge] : Indexed(std::initializer_list{ YGEdgeVertical, YGEdgeHorizontal }))
			{
				success &= SetYogaScaleProperty(
					b, params[i],
					edge,
					YGNodeStyleSetMargin,
					YGNodeStyleSetMarginPercent,
					YGNodeStyleSetMarginAuto
				);
			}
			return success;
		case 3:
			success = true;
			for (auto [i, edge] : Indexed(std::initializer_list{ YGEdgeTop, YGEdgeHorizontal, YGEdgeBottom }))
			{
				success &= SetYogaScaleProperty(
					b, params[i],
					edge,
					YGNodeStyleSetMargin,
					YGNodeStyleSetMarginPercent,
					YGNodeStyleSetMarginAuto
				);
			}
			return success;
		case 4:
			success = true;
			for (auto [i, edge] : Indexed(std::initializer_list{ YGEdgeTop, YGEdgeRight, YGEdgeBottom, YGEdgeLeft }))
			{
				success &= SetYogaScaleProperty(
					b, params[i],
					edge,
					YGNodeStyleSetMargin,
					YGNodeStyleSetMarginPercent,
					YGNodeStyleSetMarginAuto
				);
			}
			return success;
		}

		return false;
	}

	static bool SetPadding(detail::FlexBoxImpl& b, const StringView value)
	{
		bool success = false;
		auto params = String{ value }.split(U' ');

		switch (params.size())
		{
		case 1:
			return SetYogaScaleProperty(
				b, params[0],
				YGEdgeAll,
				YGNodeStyleSetPadding,
				YGNodeStyleSetPaddingPercent
			);
		case 2:
			success = true;
			for (auto [i, edge] : Indexed(std::initializer_list{ YGEdgeVertical, YGEdgeHorizontal }))
			{
				SetYogaScaleProperty(
					b, params[i],
					edge,
					YGNodeStyleSetPadding,
					YGNodeStyleSetPaddingPercent
				);
			}
			return success;
		case 3:
			success = true;
			for (auto [i, edge] : Indexed(std::initializer_list{ YGEdgeTop, YGEdgeHorizontal, YGEdgeBottom }))
			{
				SetYogaScaleProperty(
					b, params[i],
					edge,
					YGNodeStyleSetPadding,
					YGNodeStyleSetPaddingPercent
				);
			}
			return success;
		case 4:
			success = true;
			for (auto [i, edge] : Indexed(std::initializer_list{ YGEdgeTop, YGEdgeRight, YGEdgeBottom, YGEdgeLeft }))
			{
				SetYogaScaleProperty(
					b, params[i],
					edge,
					YGNodeStyleSetPadding,
					YGNodeStyleSetPaddingPercent
				);
			}
			return success;
		}

		return false;
	}

	static bool SetBorderWidth(detail::FlexBoxImpl& b, const StringView value)
	{
		bool success = false;
		auto params = String{ value }.split(U' ');

		switch (params.size())
		{
		case 1:
			return SetYogaScaleProperty(
				b, params[0],
				YGEdgeAll,
				YGNodeStyleSetBorder
			);
		case 2:
			success = true;
			for (auto [i, edge] : Indexed(std::initializer_list{ YGEdgeVertical, YGEdgeHorizontal }))
			{
				success &= SetYogaScaleProperty(
					b, params[i],
					edge,
					YGNodeStyleSetBorder,
					YGNodeStyleSetBorder
				);
			}
			return success;
		case 3:
			success = true;
			for (auto [i, edge] : Indexed(std::initializer_list{ YGEdgeTop, YGEdgeHorizontal, YGEdgeBottom }))
			{
				success &= SetYogaScaleProperty(
					b, params[i],
					edge,
					YGNodeStyleSetBorder,
					YGNodeStyleSetBorder
				);
			}
			return success;
		case 4:
			success = true;
			for (auto [i, edge] : Indexed(std::initializer_list{ YGEdgeTop, YGEdgeRight, YGEdgeBottom, YGEdgeLeft }))
			{
				success &= SetYogaScaleProperty(
					b, params[i],
					edge,
					YGNodeStyleSetBorder,
					YGNodeStyleSetBorder
				);
			}
			return success;
		}

		return false;
	}

	const static HashTable<String, YGAlign> AlignContentMap = {
		{ U"start", YGAlignFlexStart },
		{ U"end", YGAlignFlexEnd },
		{ U"stretch", YGAlignStretch },
		{ U"center", YGAlignCenter },
		{ U"space-between", YGAlignSpaceBetween },
		{ U"space-around", YGAlignSpaceAround },
		{ U"space-evenly", YGAlignSpaceEvenly },
	};

	const static HashTable<String, YGAlign> AlignItemsMap = {
		{ U"stretch", YGAlignStretch },
		{ U"start", YGAlignFlexStart },
		{ U"end", YGAlignFlexEnd },
		{ U"center", YGAlignCenter },
		{ U"baseline", YGAlignBaseline },
	};

	const static HashTable<String, YGDisplay> DisplayMap = {
		{ U"flex", YGDisplayFlex },
		{ U"none", YGDisplayNone },
	};

	const static HashTable<String, YGFlexDirection> FlexDirectionMap = {
		{ U"column", YGFlexDirectionColumn },
		{ U"row", YGFlexDirectionRow },
		{ U"row-reverse", YGFlexDirectionRowReverse },
		{ U"column-reverse", YGFlexDirectionColumnReverse },
	};

	const static HashTable<String, YGWrap> FlexWrapMap = {
		{ U"no-wrap", YGWrapNoWrap },
		{ U"wrap", YGWrapWrap },
		{ U"wrap-reverse", YGWrapWrapReverse },
	};

	const static HashTable<String, YGPositionType> PositionTypeMap = {
		{ U"static", YGPositionTypeStatic },
		{ U"relative", YGPositionTypeRelative },
		{ U"absolute", YGPositionTypeAbsolute }
	};

	const static HashTable<String, YGJustify> JustifyContentMap = {
		{ U"start", YGJustifyFlexStart },
		{ U"end", YGJustifyFlexEnd },
		{ U"center", YGJustifyCenter },
		{ U"space-between", YGJustifySpaceBetween },
		{ U"space-around", YGJustifySpaceAround },
		{ U"space-evenly", YGJustifySpaceEvenly },
	};

	const static HashTable<String, YGDirection> DirectionMap = {
		{ U"inherit", YGDirectionInherit },
		{ U"ltr", YGDirectionLTR },
		{ U"rtl", YGDirectionRTL },
	};

	using PropertySetter = bool (*)(detail::FlexBoxImpl& b, const StringView v);

	struct PropertyInfo
	{
		PropertySetter setter;

		bool inherit = false;
	};

	const static HashTable<StringView, PropertyInfo> PropertyMap = {
		{
			U"align-content"_sv,
			{ [](auto n, auto v) { return SetYogaEnumProperty(n, v, AlignContentMap, YGNodeStyleSetAlignContent); } }
		},
		{
			U"align-items"_sv,
			{ [](auto n, auto v) { return SetYogaEnumProperty(n, v, AlignItemsMap, YGNodeStyleSetAlignItems); } }
		},
		{
			U"aspect-ratio"_sv,
			{ [](auto n, auto v) { return SetYogaFloatProperty(n, v, YGNodeStyleSetAspectRatio); } }
		},
		{
			U"display"_sv,
			{ [](auto n, auto v) { return SetYogaEnumProperty(n, v, DisplayMap, YGNodeStyleSetDisplay); } }
		},
		{
			U"flex"_sv,
			{ &SetFlex }
		},
		{
			U"flex-basis"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto); } }
		},
		{
			U"flex-grow"_sv,
			{ [](auto n, auto v) { return SetYogaFloatProperty(n, v, YGNodeStyleSetFlexGrow); } }
		},
		{
			U"flex-shrink"_sv,
			{ [](auto n, auto v) { return SetYogaFloatProperty(n, v, YGNodeStyleSetFlexShrink); } }
		},
		{
			U"flex-direction"_sv,
			{ [](auto n, auto v) { return SetYogaEnumProperty(n, v, FlexDirectionMap, YGNodeStyleSetFlexDirection); } }
		},
		{
			U"flex-wrap"_sv,
			{ [](auto n, auto v) { return SetYogaEnumProperty(n, v, FlexWrapMap, YGNodeStyleSetFlexWrap); } }
		},
		{
			U"gap"_sv,
			{ &SetGap }
		},
		{
			U"row-gap"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGGutterRow, YGNodeStyleSetGap, YGNodeStyleSetGapPercent); } }
		},
		{
			U"column-gap"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGGutterColumn, YGNodeStyleSetGap, YGNodeStyleSetGapPercent); } }
		},
		{
			U"position"_sv,
			{ [](auto n, auto v) { return SetYogaEnumProperty(n, v, PositionTypeMap, YGNodeStyleSetPositionType); } }
		},
		{
			U"top"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeTop, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent); } }
		},
		{
			U"right"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeRight, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent); } }
		},
		{
			U"bottom"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeBottom, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent); } }
		},
		{
			U"left"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeLeft, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent); } }
		},
		{
			U"justify-content"_sv,
			{ [](auto n, auto v) { return SetYogaEnumProperty(n, v, JustifyContentMap, YGNodeStyleSetJustifyContent); } }
		},
		{
			U"direction"_sv,
			{ [](auto n, auto v) { return SetYogaEnumProperty(n, v, DirectionMap, YGNodeStyleSetDirection); } }
		},
		{
			U"margin"_sv,
			{ &SetMargin }
		},
		{
			U"margin-top"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeTop, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto); } }
		},
		{
			U"margin-right"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeRight, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto); } }
		},
		{
			U"margin-bottom"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeBottom, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto); } }
		},
		{
			U"margin-left"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeLeft, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto); } }
		},
		{
			U"padding"_sv,
			{ &SetPadding }
		},
		{
			U"padding-top"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeTop, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent); } }
		},
		{
			U"padding-right"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeRight, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent); } }
		},
		{
			U"padding-bottom"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeBottom, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent); } }
		},
		{
			U"padding-left"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeLeft, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent); } }
		},
		{
			U"border-width"_sv,
			{ &SetBorderWidth }
		},
		{
			U"border-top-width"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeTop, YGNodeStyleSetBorder); } }
		},
		{
			U"border-right-width"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeRight, YGNodeStyleSetBorder); } }
		},
		{
			U"border-bottom-width"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeBottom, YGNodeStyleSetBorder); } }
		},
		{
			U"border-left-width"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGEdgeLeft, YGNodeStyleSetBorder); } }
		},
		{
			U"width"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGNodeStyleSetWidth, YGNodeStyleSetWidthPercent, YGNodeStyleSetWidthAuto); } }
		},
		{
			U"height"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGNodeStyleSetHeight, YGNodeStyleSetHeightPercent, YGNodeStyleSetHeightAuto); } }
		},
		{
			U"min-width"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGNodeStyleSetMinWidth, YGNodeStyleSetMinWidthPercent); } }
		},
		{
			U"min-height"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGNodeStyleSetMinHeight, YGNodeStyleSetMinHeightPercent); } }
		},
		{
			U"max-width"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGNodeStyleSetMaxWidth, YGNodeStyleSetMaxWidthPercent); } }
		},
		{
			U"max-height"_sv,
			{ [](auto n, auto v) { return SetYogaScaleProperty(n, v, YGNodeStyleSetMaxHeight, YGNodeStyleSetMaxHeightPercent); } }
		}
	};

	bool LoadStyleToYogaNode(YGNodeRef node, const StringView key, const StringView value)
	{
		if (const auto it = PropertyMap.find(key); it != PropertyMap.end())
		{
			const PropertyInfo& info = it->second;
			info.setter(*reinterpret_cast<detail::FlexBoxImpl*>(YGNodeGetContext(node)), value);
			return true;
		}

		return false;
	}
}
