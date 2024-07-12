#pragma once
#include "LabelStatus.hpp"

namespace FlexLayout::detail
{
	class LabelRenderer
	{
	public:

		LabelRenderer(const LabelStatus& status, const StringView text)
			: status(status)
			, text(text)
		{ }

	public:

		const LabelStatus status;

		const String text;

		double fontScale() const
		{
			return status.font ? status.fontSizePixel / status.font.fontSize() : 1.0;
		}

		double lineHeight() const
		{
			return status.fontSizePixel * status.lineHeight;
		}

		size_t lineCount() const
		{
			return m_lineWidths.size();
		}

		double baseline(size_t lineIdx = Largest<size_t>) const
		{
			lineIdx = std::min(lineIdx, lineCount() - 1);
			auto scale = fontScale();
			auto line = lineHeight();
			auto content = status.font.height() * scale;
			auto ascender = status.font.ascender() * scale;
			return line * lineIdx
				+ (line - content) / 2
				+ ascender;
		}

		Vec2 size() const
		{
			return {
				*std::max_element(m_lineWidths.begin(), m_lineWidths.end()),
				lineHeight() * lineCount()
			};
		}

		void setConstraints(double width = Inf<double>);

		void draw(RectF rect, const ColorF color) const;

	private:

		Array<Glyph> m_glyphs;

		Array<size_t> m_lineBreaks;

		Array<double> m_lineWidths;
	};
}
