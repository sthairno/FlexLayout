#include "Button.hpp"

using namespace s3d;

namespace FlexLayout::SimpleGUI
{
	void Button::attach(UIStateQuery query)
	{
		auto size = s3d::SimpleGUI::ButtonRegion(query.textContent(), { 0, 0 }).size;

		query.setStyle(
			U"min-width",
			StyleValue::Length(static_cast<float>(size.x), LengthUnit::Pixel)
		);
		query.setStyle(
			U"height",
			StyleValue::Length(static_cast<float>(size.y), LengthUnit::Pixel)
		);
	}

	void Button::draw(UIStateQuery query, const Box& box)
	{
		m_clicked = false;
		auto rect = box.contentAreaRect();
		if (rect)
		{
			m_clicked = s3d::SimpleGUI::Button(
				query.textContent(),
				rect->pos,
				rect->w,
				m_enabled
			);
		}
	}

	void Button::setTextContent(UIStateQuery query, s3d::StringView text)
	{
		auto size = s3d::SimpleGUI::ButtonRegion(text, { 0, 0 }).size;

		query.setStyle(
			U"min-width",
			StyleValue::Length(static_cast<float>(size.x), LengthUnit::Pixel)
		);
		query.setStyle(
			U"height",
			StyleValue::Length(static_cast<float>(size.y), LengthUnit::Pixel)
		);
	}

	std::unique_ptr<UIState> Button::clone()
	{
		auto ptr = std::make_unique<Button>();

		ptr->m_enabled = m_enabled;

		return ptr;
	}
}
