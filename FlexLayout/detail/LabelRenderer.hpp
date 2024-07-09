#pragma once
#include "LabelStatus.hpp"

namespace FlexLayout::detail
{
	class LabelRenderer
	{
	public:

		LabelRenderer(const LabelStatus& m_status, const StringView text)
			: m_status(m_status)
			, m_text(text)
		{ }

	public:

		double fontScale() const
		{
			return m_status.font ? m_status.fontSizePixel / m_status.font.fontSize() : 1.0;
		}

		double lineScale() const
		{
			return fontScale() * m_status.lineHeight;
		}

		size_t lineCount() const
		{
			return m_lineWidths.size();
		}

		double baseline(size_t lineIdx = Largest<size_t>) const
		{
			lineIdx = std::min(lineIdx, lineCount() - 1);
			return (m_status.font.height() * lineIdx + m_status.font.ascender()) * lineScale();
		}

		Vec2 size() const
		{
			return {
				*std::max_element(m_lineWidths.begin(), m_lineWidths.end()),
				lineCount() * m_status.font.height() * lineScale()
			};
		}

		void build(double width = Inf<double>);

		void draw(RectF rect, const ColorF color) const;

	private:

		const LabelStatus m_status;

		String m_text;

		Array<Glyph> m_glyphs;

		Array<size_t> m_lineBreaks;

		Array<double> m_lineWidths;
	};
}
