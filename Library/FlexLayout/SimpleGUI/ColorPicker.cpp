#include "ColorPicker.hpp"

using namespace s3d;

namespace FlexLayout::SimpleGUI
{
	void ColorPicker::attach(UIStateQuery query)
	{
		auto size = s3d::SimpleGUI::ColorPickerRegion({ 0, 0 }).size;
		query.setStyle(
			U"width",
			StyleValue::Length(static_cast<float>(size.x), LengthUnit::Pixel)
		);
		query.setStyle(
			U"height",
			StyleValue::Length(static_cast<float>(size.y), LengthUnit::Pixel)
		);
	}

	void ColorPicker::draw(UIStateQuery, const Box& box)
	{
		m_changed = false;
		if (auto rect = box.contentAreaRect())
		{
			m_changed = s3d::SimpleGUI::ColorPicker(
				m_value,
				rect->pos,
				m_enabled
			);
		}
	}

	std::unique_ptr<UIState> ColorPicker::clone()
	{
		auto ptr = std::make_unique<ColorPicker>();

		ptr->m_value = m_value;
		ptr->m_enabled = m_enabled;

		return ptr;
	}
}
