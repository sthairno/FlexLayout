#include "LoadStyleToYogaNode.hpp"

using namespace std::placeholders;

template<typename ValueT, typename SetterFuncT>
static bool SetEnum(
	YGNodeRef node,
	const HashTable<String, ValueT>& valueMapper,
	const StringView value,
	SetterFuncT&& setter)
{
	if (const auto it = valueMapper.find(value); it != valueMapper.end())
	{
		setter(node, it->second);
		return true;
	}

	return false;
}

template<typename SetterT>
static bool SetFloat(
	YGNodeRef node,
	const StringView value,
	SetterT&& setter)
{
	if (auto v = ParseFloatOpt<float>(value))
	{
		setter(node, *v);
		return true;
	}

	return false;
}

template<typename SetterPixelT>
static bool SetScale(
	YGNodeRef node,
	const StringView value,
	SetterPixelT&& setterPixels)
{
	if (value.ends_with(U"px"_sv))
	{
		if (auto v = ParseFloatOpt<float>(value.substr(0, value.size() - 2)))
		{
			setterPixels(node, *v);
			return true;
		}
	}
	else
	{
		if (auto v = ParseFloatOpt<float>(value))
		{
			setterPixels(node, *v);
			return true;
		}
	}

	return false;
}

template<typename SetterPixelT, typename SetterPercentT>
static bool SetScale(
	YGNodeRef node,
	const StringView value,
	SetterPixelT&& setterPixels,
	SetterPercentT&& setterPercentage)
{
	if (value.ends_with(U"%"_sv))
	{
		if (auto v = ParseFloatOpt<float>(value.substr(0, value.size() - 1)))
		{
			setterPercentage(node, *v);
			return true;
		}
	}

	return SetScale(node, value, setterPixels);
}

template<typename SetterPixelT, typename SetterPercentT, typename SetterAutoT>
static bool SetScale(
	YGNodeRef node,
	const StringView value,
	SetterPixelT&& setterPixels,
	SetterPercentT&& setterPercentage,
	SetterAutoT&& setterAuto)
{
	if (value == U"auto")
	{
		setterAuto(node);
		return true;
	}

	return SetScale(node, value, setterPixels, setterPercentage);
}

template<typename ParamT, typename SetterPixelT>
static bool SetScaleWithParam(
	YGNodeRef node,
	ParamT param,
	const StringView value,
	SetterPixelT&& setterPixels)
{
	if (value.ends_with(U"px"_sv))
	{
		if (auto v = ParseFloatOpt<float>(value.substr(0, value.size() - 2)))
		{
			setterPixels(node, param, *v);
			return true;
		}
	}
	else
	{
		if (auto v = ParseFloatOpt<float>(value))
		{
			setterPixels(node, param, *v);
			return true;
		}
	}

	return false;
}

template<typename ParamT, typename SetterPixelT, typename SetterPercentT>
static bool SetScaleWithParam(
	YGNodeRef node,
	ParamT param,
	const StringView value,
	SetterPixelT&& setterPixels,
	SetterPercentT&& setterPercentage)
{
	if (value.ends_with(U"%"_sv))
	{
		if (auto v = ParseFloatOpt<float>(value.substr(0, value.size() - 1)))
		{
			setterPercentage(node, param, *v);
			return true;
		}
	}

	return SetScaleWithParam(node, param, value, setterPixels);
}

template<typename ParamT, typename SetterPixelT, typename SetterPercentT, typename SetterAutoT>
static bool SetScaleWithParam(
	YGNodeRef node,
	ParamT param,
	const StringView value,
	SetterPixelT&& setterPixels,
	SetterPercentT&& setterPercentage,
	SetterAutoT&& setterAuto)
{
	if (value == U"auto")
	{
		setterAuto(node, param);
		return true;
	}

	return SetScaleWithParam(node, param, value, setterPixels, setterPercentage);
}

static void SetFlex(YGNodeRef node, const StringView value)
{
	auto params = String{ value }.split(U' ');

	switch (params.size())
	{
	case 1:
		if (params[0] == U"initial")
		{
			YGNodeStyleSetFlexGrow(node, 0);
			YGNodeStyleSetFlexShrink(node, 1);
			YGNodeStyleSetFlexBasisAuto(node);
		}
		else if (params[0] == U"auto")
		{
			YGNodeStyleSetFlexGrow(node, 1);
			YGNodeStyleSetFlexShrink(node, 1);
			YGNodeStyleSetFlexBasisAuto(node);
		}
		else if (params[0] == U"none")
		{
			YGNodeStyleSetFlexGrow(node, 0);
			YGNodeStyleSetFlexShrink(node, 0);
			YGNodeStyleSetFlexBasisAuto(node);
		}
		else if (SetFloat(node, params[0], YGNodeStyleSetFlexGrow))
		{
			YGNodeStyleSetFlexShrink(node, 1);
			YGNodeStyleSetFlexBasis(node, 0);
		}
		else if (SetScale(node, params[0], YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto))
		{
			YGNodeStyleSetFlexGrow(node, 1);
			YGNodeStyleSetFlexShrink(node, 1);
		}
		break;
	case 2:
		SetFloat(node, params[0], YGNodeStyleSetFlexGrow);
		if (SetFloat(node, params[0], YGNodeStyleSetFlexShrink))
		{
			YGNodeStyleSetFlexBasis(node, 0);
		}
		else if (SetScale(node, params[1], YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto))
		{
			YGNodeStyleSetFlexShrink(node, 1);
		}
		break;
	case 3:
		SetFloat(node, params[0], YGNodeStyleSetFlexGrow);
		SetFloat(node, params[1], YGNodeStyleSetFlexShrink);
		SetScale(node, params[2], YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto);
		break;
	}
}

static void SetGap(YGNodeRef node, const StringView value)
{
	auto params = String{ value }.split(U' ');

	switch (params.size())
	{
	case 1:
		SetScaleWithParam(
			node, YGGutterAll, params[0],
			YGNodeStyleSetGap,
			YGNodeStyleSetGapPercent
		);
		break;
	case 2:
		SetScaleWithParam(
			node, YGGutterRow, params[0],
			YGNodeStyleSetGap,
			YGNodeStyleSetGapPercent
		);
		SetScaleWithParam(
			node, YGGutterColumn, params[1],
			YGNodeStyleSetGap,
			YGNodeStyleSetGapPercent
		);
		break;
	}
}

static void SetMargin(YGNodeRef node, const StringView value)
{
	auto params = String{ value }.split(U' ');

	switch (params.size())
	{
	case 1:
		SetScaleWithParam(
			node, YGEdgeAll, params[0],
			YGNodeStyleSetMargin,
			YGNodeStyleSetMarginPercent,
			YGNodeStyleSetMarginAuto
		);
		break;
	case 2:
		for (auto [i, e] : Indexed(std::initializer_list{ YGEdgeVertical, YGEdgeHorizontal }))
		{
			SetScaleWithParam(
				node, e, params[i],
				YGNodeStyleSetMargin,
				YGNodeStyleSetMarginPercent,
				YGNodeStyleSetMarginAuto
			);
		}
		break;
	case 3:
		for (auto [i, e] : Indexed(std::initializer_list{ YGEdgeTop, YGEdgeHorizontal, YGEdgeBottom }))
		{
			SetScaleWithParam(
				node, e, params[i],
				YGNodeStyleSetMargin,
				YGNodeStyleSetMarginPercent,
				YGNodeStyleSetMarginAuto
			);
		}
		break;
	case 4:
		for (auto [i, e] : Indexed(std::initializer_list{ YGEdgeTop, YGEdgeRight, YGEdgeBottom, YGEdgeLeft }))
		{
			SetScaleWithParam(
				node, e, params[i],
				YGNodeStyleSetMargin,
				YGNodeStyleSetMarginPercent,
				YGNodeStyleSetMarginAuto
			);
		}
		break;
	}
}

static void SetPadding(YGNodeRef node, const StringView value)
{
	auto params = String{ value }.split(U' ');

	switch (params.size())
	{
	case 1:
		SetScaleWithParam(
			node, YGEdgeAll, params[0],
			YGNodeStyleSetPadding,
			YGNodeStyleSetPaddingPercent
		);
		break;
	case 2:
		for (auto [i, e] : Indexed(std::initializer_list{ YGEdgeVertical, YGEdgeHorizontal }))
		{
			SetScaleWithParam(
				node, e, params[i],
				YGNodeStyleSetPadding,
				YGNodeStyleSetPaddingPercent
			);
		}
		break;
	case 3:
		for (auto [i, e] : Indexed(std::initializer_list{ YGEdgeTop, YGEdgeHorizontal, YGEdgeBottom }))
		{
			SetScaleWithParam(
				node, e, params[i],
				YGNodeStyleSetPadding,
				YGNodeStyleSetPaddingPercent
			);
		}
		break;
	case 4:
		for (auto [i, e] : Indexed(std::initializer_list{ YGEdgeTop, YGEdgeRight, YGEdgeBottom, YGEdgeLeft }))
		{
			SetScaleWithParam(
				node, e, params[i],
				YGNodeStyleSetPadding,
				YGNodeStyleSetPaddingPercent
			);
		}
		break;
	}

}

static void SetBorderWidth(YGNodeRef node, const StringView value)
{
	auto params = String{ value }.split(U' ');

	switch (params.size())
	{
	case 1:
		SetScaleWithParam(
			node, YGEdgeAll, params[0],
			YGNodeStyleSetBorder
		);
		break;
	case 2:
		for (auto [i, e] : Indexed(std::initializer_list{ YGEdgeVertical, YGEdgeHorizontal }))
		{
			SetScaleWithParam(
				node, e, params[i],
				YGNodeStyleSetBorder,
				YGNodeStyleSetBorder
			);
		}
		break;
	case 3:
		for (auto [i, e] : Indexed(std::initializer_list{ YGEdgeTop, YGEdgeHorizontal, YGEdgeBottom }))
		{
			SetScaleWithParam(
				node, e, params[i],
				YGNodeStyleSetBorder,
				YGNodeStyleSetBorder
			);
		}
		break;
	case 4:
		for (auto [i, e] : Indexed(std::initializer_list{ YGEdgeTop, YGEdgeRight, YGEdgeBottom, YGEdgeLeft }))
		{
			SetScaleWithParam(
				node, e, params[i],
				YGNodeStyleSetBorder,
				YGNodeStyleSetBorder
			);
		}
		break;
	}
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

const static HashTable<StringView, std::function<void(YGNodeRef, const StringView)>> FuncMap = {
	{ U"align-content"_sv, [](YGNodeRef n, const StringView v) { SetEnum(n, AlignContentMap, v, YGNodeStyleSetAlignContent); }},
	{ U"align-items"_sv, [](YGNodeRef n, const StringView v) { SetEnum(n, AlignItemsMap, v, YGNodeStyleSetAlignItems); }},
	{ U"aspect-ratio"_sv, [](YGNodeRef n, const StringView v) { SetFloat(n, v, YGNodeStyleSetAspectRatio); }},
	{ U"display"_sv, [](YGNodeRef n, const StringView v) { SetEnum(n, DisplayMap, v, YGNodeStyleSetDisplay); }},
	{ U"flex"_sv, SetFlex },
	{ U"flex-basis"_sv, [](YGNodeRef n, const StringView v) { SetScale(n, v, YGNodeStyleSetFlexBasis, YGNodeStyleSetFlexBasisPercent, YGNodeStyleSetFlexBasisAuto); }},
	{ U"flex-grow"_sv, [](YGNodeRef n, const StringView v) { SetFloat(n, v, YGNodeStyleSetFlexGrow); }},
	{ U"flex-shrink"_sv, [](YGNodeRef n, const StringView v) { SetFloat(n, v, YGNodeStyleSetFlexShrink); }},
	{ U"flex-direction"_sv, [](YGNodeRef n, const StringView v) { SetEnum(n, FlexDirectionMap, v, YGNodeStyleSetFlexDirection); }},
	{ U"flex-wrap"_sv, [](YGNodeRef n, const StringView v) { SetEnum(n, FlexWrapMap, v, YGNodeStyleSetFlexWrap); }},
	{ U"gap"_sv, SetGap },
	{ U"row-gap"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGGutterRow, v, YGNodeStyleSetGap, YGNodeStyleSetGapPercent); }},
	{ U"column-gap"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGGutterColumn, v, YGNodeStyleSetGap, YGNodeStyleSetGapPercent); }},
	{ U"position"_sv, [](YGNodeRef n, const StringView v) { SetEnum(n, PositionTypeMap, v, YGNodeStyleSetPositionType); }},
	{ U"top"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeTop, v, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent); }},
	{ U"right"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeRight, v, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent); }},
	{ U"bottom"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeBottom, v, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent); }},
	{ U"left"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeLeft, v, YGNodeStyleSetPosition, YGNodeStyleSetPositionPercent); }},
	{ U"justify-content"_sv, [](YGNodeRef n, const StringView v) { SetEnum(n, JustifyContentMap, v, YGNodeStyleSetJustifyContent); }},
	{ U"direction"_sv, [](YGNodeRef n, const StringView v) { SetEnum(n, DirectionMap, v, YGNodeStyleSetDirection); }},
	{ U"margin"_sv, SetMargin },
	{ U"margin-top"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeTop, v, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto); }},
	{ U"margin-right"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeRight, v, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto); }},
	{ U"margin-bottom"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeBottom, v, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto); }},
	{ U"margin-left"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeLeft, v, YGNodeStyleSetMargin, YGNodeStyleSetMarginPercent, YGNodeStyleSetMarginAuto); }},
	{ U"padding"_sv, SetPadding },
	{ U"padding-top"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeTop, v, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent); }},
	{ U"padding-right"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeRight, v, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent); }},
	{ U"padding-bottom"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeBottom, v, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent); }},
	{ U"padding-left"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeLeft, v, YGNodeStyleSetPadding, YGNodeStyleSetPaddingPercent); }},
	{ U"border-width"_sv, SetBorderWidth },
	{ U"border-top-width"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeTop, v, YGNodeStyleSetBorder); }},
	{ U"border-right-width"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeRight, v, YGNodeStyleSetBorder); }},
	{ U"border-bottom-width"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeBottom, v, YGNodeStyleSetBorder); }},
	{ U"border-left-width"_sv, [](YGNodeRef n, const StringView v) { SetScaleWithParam(n, YGEdgeLeft, v, YGNodeStyleSetBorder); }},
	{ U"width"_sv, [](YGNodeRef n, const StringView v) { SetScale(n, v, YGNodeStyleSetWidth, YGNodeStyleSetWidthPercent, YGNodeStyleSetWidthAuto); }},
	{ U"height"_sv, [](YGNodeRef n, const StringView v) { SetScale(n, v, YGNodeStyleSetHeight, YGNodeStyleSetHeightPercent, YGNodeStyleSetHeightAuto); }},
	{ U"min-width"_sv, [](YGNodeRef n, const StringView v) { SetScale(n, v, YGNodeStyleSetMinWidth, YGNodeStyleSetMinWidthPercent); }},
	{ U"min-height"_sv, [](YGNodeRef n, const StringView v) { SetScale(n, v, YGNodeStyleSetMinHeight, YGNodeStyleSetMinHeightPercent); }},
	{ U"max-width"_sv, [](YGNodeRef n, const StringView v) { SetScale(n, v, YGNodeStyleSetMaxWidth, YGNodeStyleSetMaxWidthPercent); }},
	{ U"max-height"_sv, [](YGNodeRef n, const StringView v) { SetScale(n, v, YGNodeStyleSetMaxHeight, YGNodeStyleSetMaxHeightPercent); }},
};

bool FlexLayout::Util::LoadStyleToYogaNode(YGNodeRef node, const StringView key, const StringView value)
{
	if (const auto it = FuncMap.find(key); it != FuncMap.end())
	{
		it->second(node, value);
		return true;
	}

	return false;
}
