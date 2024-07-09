#include "LabelRenderer.hpp"

namespace FlexLayout::detail
{
	void LabelRenderer::build(double width)
	{
		m_glyphs.clear();
		m_lineBreaks.clear();
		m_lineWidths.clear();

		if (m_text.isEmpty())
		{
			return;
		}
		m_glyphs = m_status.font.getGlyphs(m_text);

		auto scale = fontScale();

		m_lineWidths.push_back(0.0);
		for (auto [i, glyph] : Indexed(m_glyphs))
		{
			auto& lineWidth = m_lineWidths.back();

			if (glyph.codePoint == U'\n')
			{
				m_lineBreaks.push_back(i + 1);
				m_lineWidths.push_back(0.0);
				continue;
			}

			double xadvance = IsControl(glyph.codePoint) ? 0.0 : glyph.xAdvance * scale;

			if (lineWidth > 0.0 && lineWidth + xadvance > width)
			{
				m_lineBreaks.push_back(i);
				m_lineWidths.push_back(xadvance);
				continue;
			}

			lineWidth += xadvance;
		}
	}

	void LabelRenderer::draw(RectF rect, const ColorF color) const
	{
		const auto& pixelShader = Font::GetPixelShader(m_status.font.method());
		const ScopedCustomShader2D shader{ pixelShader };

		double fontScale = this->fontScale();

		size_t beginIdx = 0;
		auto nextLineBreakItr = m_lineBreaks.cbegin();

		for (auto [lineIdx, lineWidth] : Indexed(m_lineWidths))
		{
			const size_t endIdx = nextLineBreakItr != m_lineBreaks.cend() ? *nextLineBreakItr++ : m_glyphs.size();

			Vec2 penPos = rect.pos;
			switch(m_status.textAlign)
			{
			case TextAlign::Inherit:
			case TextAlign::Left:
				break;
			case TextAlign::Center:
				penPos.x += (rect.w - lineWidth) / 2;
				break;
			case TextAlign::Right:
				penPos.x += rect.w - lineWidth;
				break;
			}
			penPos.y += baseline(lineIdx);

			for (size_t glyphIdx : Iota(beginIdx, endIdx))
			{
				const auto& glyph = m_glyphs[glyphIdx];

				if (not IsControl(glyph.codePoint))
				{
					const Vec2 glyphPos = penPos + glyph.getBase(fontScale);
					glyph.texture.scaled(fontScale, fontScale).draw(glyphPos, color);
					penPos.x += glyph.xAdvance * fontScale;
				}
			}

			beginIdx = endIdx;
		}
	}
}
