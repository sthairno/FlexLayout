#pragma once
#include <Siv3D/Font.hpp>
#include <Siv3D/SimpleGUI.hpp>
#include "../Enum/TextAlign.hpp"

using namespace s3d;

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
			return font ? fontSizePx / font.fontSize() : 1.0F;
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

	[[nodiscard]]
	inline bool operator==(const ComputedTextStyle& lhs, const ComputedTextStyle& rhs)
	{
		return lhs.font == rhs.font &&
			lhs.fontSizePx == rhs.fontSizePx &&
			lhs.lineHeightMul == rhs.lineHeightMul &&
			lhs.textAlign == rhs.textAlign;
	}

	[[nodiscard]]
	inline bool operator!=(const ComputedTextStyle& lhs, const ComputedTextStyle& rhs)
	{
		return !(lhs == rhs);
	}
}
