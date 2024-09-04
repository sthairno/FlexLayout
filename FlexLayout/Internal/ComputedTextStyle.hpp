#pragma once
#include "Common.hpp"
#include "../Enum/TextAlign.hpp"

namespace FlexLayout::Internal
{
	struct ComputedTextStyle
	{
		Font font = SimpleGUI::GetFont();

		float fontSizePx = 16.0F;

		float lineHeightMul = 1.2F;

		TextAlign textAlign = TextAlign::Start;

		float fontRenderingScale() const
		{
			return font ? fontSizePx / font.fontSize() : 1.0;
		}

		float zeroGlyphAdvancePx() const
		{
			return static_cast<float>(font.getGlyphInfo(U'0').xAdvance * fontRenderingScale());
		}

		float xGlyphHeightPx() const
		{
			return font.getGlyphInfo(U'x').height * fontRenderingScale();
		}

		float cjkWaterGlyphAdvancePx() const
		{
			return static_cast<float>(font.getGlyphInfo(U'水').xAdvance * fontRenderingScale());
		}

		float lineHeightPx() const
		{
			return fontSizePx * lineHeightMul;
		}
	};
}
