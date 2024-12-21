#include "TextArea.hpp"
#include <Siv3D/Parse.hpp>

using namespace s3d;

namespace FlexLayout::SimpleGUI
{
	static constexpr size_t TextAreaDefaultMaxChars = 800;

	TextArea::TextArea()
		: m_state{ std::make_unique<TextAreaEditState>() }
		, m_maxChars(TextAreaDefaultMaxChars)
	{ }

	TextArea::TextArea(StringView text)
		: m_state{ std::make_unique<TextAreaEditState>(String{ text }) }
		, m_maxChars(TextAreaDefaultMaxChars)
	{ }

	void TextArea::attach(UIStateQuery query)
	{
		auto size = s3d::SimpleGUI::TextAreaRegion(
			{ 0, 0 }, { 0, 0 }
		).size;

		query.setStyle(
			U"min-width",
			StyleValue::Length(static_cast<float>(size.x), LengthUnit::Pixel)
		);
		query.setStyle(
			U"min-height",
			StyleValue::Length(static_cast<float>(size.y), LengthUnit::Pixel)
		);
	}

	void TextArea::draw(UIStateQuery, const Box& box)
	{
		if (auto rect = box.contentAreaRect())
		{
			s3d::SimpleGUI::TextArea(
				*m_state,
				rect->pos,
				rect->size,
				m_maxChars,
				m_enabled
			);
		}
	}

	void TextArea::setProperty(UIStateQuery, s3d::StringView key, s3d::StringView value)
	{
		if (key == U"maxChars")
		{
			m_maxChars = ParseOpt<size_t>(value)
				.value_or(TextAreaDefaultMaxChars);
		}
	}

	void TextArea::unsetProperty(UIStateQuery, s3d::StringView key)
	{
		if (key == U"maxChars")
		{
			m_maxChars = TextAreaDefaultMaxChars;
		}
	}

	void TextArea::setTextContent(UIStateQuery, s3d::StringView text)
	{
		m_state = std::make_unique<TextAreaEditState>(String{ text });
	}

	std::unique_ptr<UIState> TextArea::clone()
	{
		auto ptr = std::make_unique<TextArea>(m_state->text);
		
		ptr->m_maxChars = m_maxChars;
		ptr->m_enabled = m_enabled;

		return ptr;
	}
}
