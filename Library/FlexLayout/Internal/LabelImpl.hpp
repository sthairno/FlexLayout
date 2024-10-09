#pragma once
#include <Siv3D/StringView.hpp>
#include <Siv3D/TextStyle.hpp>
#include <Siv3D/ColorF.hpp>
#include <Siv3D/Glyph.hpp>
#include "FlexBoxImpl.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	class LabelImpl : public FlexBoxImpl
	{
	public:

		LabelImpl(std::shared_ptr<TreeContext> context, const StringView tagName);

		LabelImpl(const LabelImpl& source, std::shared_ptr<TreeContext> newContext);

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

		bool m_layoutIsValid = false;

		size_t lineCount() const { return m_lineWidths.size(); }

		void updateConstraints(double width);

		double computeBaseline(size_t lineIdx = Largest<size_t>) const;

		SizeF computeBoundingBox() const;
	};
}
