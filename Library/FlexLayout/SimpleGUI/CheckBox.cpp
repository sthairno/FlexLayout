#include "CheckBox.hpp"

using namespace s3d;

namespace FlexLayout::SimpleGUI
{
	void CheckBox::attach(UIStateQuery query)
	{
		auto size = s3d::SimpleGUI::CheckBoxRegion(query.textContent(), { 0, 0 }).size;
		query.setStyle(
			U"min-width",
			StyleValue::Length(static_cast<float>(size.x), LengthUnit::Pixel)
		);
		query.setStyle(
			U"height",
			StyleValue::Length(static_cast<float>(size.y), LengthUnit::Pixel)
		);
	}

	void CheckBox::draw(UIStateQuery query, const Box& box)
	{
		m_changed = false;
		auto rect = box.contentAreaRect();
		if (rect)
		{
			m_changed = s3d::SimpleGUI::CheckBox(
				m_checked,
				query.textContent(),
				rect->pos,
				rect->w,
				m_enabled
			);
		}
	}

	void CheckBox::setTextContent(UIStateQuery query, s3d::StringView text)
	{
		auto size = s3d::SimpleGUI::CheckBoxRegion(text, { 0, 0 }).size;
		query.setStyle(
			U"min-width",
			StyleValue::Length(static_cast<float>(size.x), LengthUnit::Pixel)
		);
		query.setStyle(
			U"height",
			StyleValue::Length(static_cast<float>(size.y), LengthUnit::Pixel)
		);
	}

	std::unique_ptr<UIState> CheckBox::clone()
	{
		auto ptr = std::make_unique<CheckBox>();

		ptr->m_checked = m_checked;
		ptr->m_enabled = m_enabled;

		return ptr;
	}
}
