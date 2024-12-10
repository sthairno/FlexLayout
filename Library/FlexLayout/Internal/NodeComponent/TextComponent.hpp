#pragma once
#include <Siv3D/String.hpp>
#include <Siv3D/Array.hpp>
#include <Siv3D/HashTable.hpp>
#include <Siv3D/Optional.hpp>
#include <Siv3D/Vector2D.hpp>
#include <Siv3D/Glyph.hpp>
#include <Siv3D/TextStyle.hpp>

#include "StyleComponent.hpp"
#include "LayoutComponent.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	class FlexBoxNode;
}

namespace FlexLayout::Internal::Component
{
	class TextComponent
	{
	public:

		TextComponent(FlexBoxNode& node);

	public:

		void copy(const TextComponent& source);

		const StringView text() const;

		void setText(const StringView text);

		void draw(const TextStyle& textStyle, const ColorF& color);

	private:

		struct Impl;

		FlexBoxNode& m_node;

		String m_text;

		Array<Glyph> m_glyphs;

		Array<size_t> m_lineBreaks;

		Array<double> m_lineWidths;

		bool m_layoutIsValid = false;

		StyleComponent& styleComponent();

		const StyleComponent& styleComponent() const;

		LayoutComponent& layoutComponent();

		const LayoutComponent& layoutComponent() const;

		size_t lineCount() const { return m_lineWidths.size(); }

		void updateConstraints(double width);

		double computeBaseline(size_t lineIdx = Largest<size_t>) const;

		SizeF computeBoundingBox() const;
	};
}
