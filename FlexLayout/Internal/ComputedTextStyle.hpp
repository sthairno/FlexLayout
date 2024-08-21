﻿#pragma once
#include "Common.hpp"
#include "../Enum/TextAlign.hpp"

namespace FlexLayout::Internal
{
	struct ComputedTextStyle
	{
		Font font = SimpleGUI::GetFont();
		double fontSizePixel = 16.0;
		double lineHeightMul = 1.2;
		TextAlign textAlign = TextAlign::Left;
	};
}
