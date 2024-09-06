#pragma once
#include "Common.hpp"
#include "FlexBoxImpl.hpp"

namespace FlexLayout::Internal
{
	class LabelImpl : public FlexBoxImpl
	{
	public:

		LabelImpl(std::shared_ptr<TreeContext> context, const StringView tagName);

	public:

		NodeType type() const noexcept override { return NodeType::Label; }

		const StringView text() const { return m_text; }

		void setText(const StringView text);

		void draw(const TextStyle& textStyle, const ColorF& color);

	private:

		struct Impl;

		String m_text;

		Array<Glyph> m_glyphs;

		Array<size_t> m_lineBreaks;

		Array<double> m_lineWidths;

		size_t lineCount() const { return m_lineWidths.size(); }

		void updateConstraints(double width);

		double computeBaseline(size_t lineIdx = Largest<size_t>) const;

		SizeF computeBoundingBox() const;
	};
}
