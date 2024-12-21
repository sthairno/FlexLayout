#include "TextBox.hpp"
#include <Siv3D/Parse.hpp>

using namespace s3d;

namespace FlexLayout::SimpleGUI
{
	TextBox::TextBox()
		: m_state{ std::make_unique<TextEditState>() }
	{ }

	TextBox::TextBox(StringView text)
		: m_state{ std::make_unique<TextEditState>(String{ text }) }
	{ }

	void TextBox::attach(UIStateQuery query)
	{
		auto size = s3d::SimpleGUI::TextBoxRegion(
			{ 0, 0 }, 0
		).size;

		query.setStyle(
			U"min-width",
			StyleValue::Length(static_cast<float>(size.x), LengthUnit::Pixel)
		);
		query.setStyle(
			U"min-height",
			StyleValue::Length(static_cast<float>(size.y), LengthUnit::Pixel)
		);
		query.setStyle(
			U"height",
			StyleValue::Length(static_cast<float>(size.y), LengthUnit::Pixel)
		);
	}

	void TextBox::draw(UIStateQuery, const Box& box)
	{
		if (auto rect = box.contentAreaRect())
		{
			s3d::SimpleGUI::TextBox(
				*m_state,
				rect->pos,
				rect->w,
				m_maxChars,
				m_enabled
			);
		}
	}

	void TextBox::setProperty(UIStateQuery, s3d::StringView key, s3d::StringView value)
	{
		if (key == U"maxChars")
		{
			m_maxChars = ParseOpt<size_t>(value);
		}
	}

	void TextBox::unsetProperty(UIStateQuery, s3d::StringView key)
	{
		if (key == U"maxChars")
		{
			m_maxChars.reset();
		}
	}

	void TextBox::setTextContent(UIStateQuery, s3d::StringView text)
	{
		m_state = std::make_unique<TextEditState>(String{ text });
	}

	std::unique_ptr<UIState> TextBox::clone()
	{
		auto ptr = std::make_unique<TextBox>(m_state->text);

		ptr->m_maxChars = m_maxChars;
		ptr->m_enabled = m_enabled;

		return ptr;
	}
}
