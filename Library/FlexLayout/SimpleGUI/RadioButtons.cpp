#include "RadioButtons.hpp"

using namespace s3d;

namespace FlexLayout::SimpleGUI
{
	void RadioButtons::attach(UIStateQuery query)
	{
		auto size = s3d::SimpleGUI::RadioButtonsRegion(
			m_options,
			{ 0, 0 }
		).size;

		query.setStyle(
			U"min-width",
			StyleValue::Length(static_cast<float>(size.x), LengthUnit::Pixel)
		);
		query.setStyle(
			U"height",
			StyleValue::Length(static_cast<float>(size.y), LengthUnit::Pixel)
		);
	}

	void RadioButtons::draw(UIStateQuery, const Box& box)
	{
		m_changed = false;
		if (auto rect = box.contentAreaRect())
		{
			m_changed = s3d::SimpleGUI::RadioButtons(
				m_index,
				m_options,
				rect->pos,
				rect->w,
				m_enabled
			);
		}
	}

	void RadioButtons::setTextContent(UIStateQuery query, s3d::StringView text)
	{
		m_options = String{ text }.split_lines();
		if (m_index >= m_options.size())
		{
			m_index = 0;
		}

		auto size = s3d::SimpleGUI::RadioButtonsRegion(
			m_options,
			{ 0, 0 }
		).size;

		query.setStyle(
			U"min-width",
			StyleValue::Length(static_cast<float>(size.x), LengthUnit::Pixel)
		);
		query.setStyle(
			U"height",
			StyleValue::Length(static_cast<float>(size.y), LengthUnit::Pixel)
		);
	}

	std::unique_ptr<UIState> RadioButtons::clone()
	{
		auto ptr = std::make_unique<RadioButtons>();

		ptr->m_options = m_options;
		ptr->m_index = m_index;
		ptr->m_enabled = m_enabled;

		return ptr;
	}
}
