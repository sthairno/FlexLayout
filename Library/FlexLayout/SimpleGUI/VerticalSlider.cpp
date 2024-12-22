#include "VerticalSlider.hpp"
#include <Siv3D/Parse.hpp>

using namespace s3d;

namespace FlexLayout::SimpleGUI
{
	void VerticalSlider::attach(UIStateQuery query)
	{
		auto size = s3d::SimpleGUI::VerticalSliderRegion(
			{ 0, 0 },
			0.0
		).size;

		query.setStyle(
			U"min-width",
			StyleValue::Length(static_cast<float>(size.x), LengthUnit::Pixel)
		);
		query.setStyle(
			U"width",
			StyleValue::Length(static_cast<float>(size.x), LengthUnit::Pixel)
		);
		query.setStyle(
			U"min-height",
			StyleValue::Length(static_cast<float>(size.y), LengthUnit::Pixel)
		);
	}

	void VerticalSlider::draw(UIStateQuery, const Box& box)
	{
		m_changed = false;
		if (auto rect = box.contentAreaRect())
		{
			m_changed = s3d::SimpleGUI::VerticalSlider(
				m_value,
				m_min,
				m_max,
				rect->pos,
				rect->h,
				m_enabled
			);
		}
	}

	void VerticalSlider::setProperty(UIStateQuery, s3d::StringView key, s3d::StringView value)
	{
		if (key == U"min")
		{
			m_min = ParseOpt<double>(value).value_or(0.0);
			m_value = Max(m_value, m_min);
		}
		else if (key == U"max")
		{
			m_max = ParseOpt<double>(value).value_or(1.0);
			m_value = Max(Min(m_value, m_max), m_min);
		}
		else if (key == U"value")
		{
			m_value = ParseOpt<double>(value).value_or(0.0);
			m_value = Max(Min(m_value, m_max), m_min);
		}
	}

	void VerticalSlider::unsetProperty(UIStateQuery, s3d::StringView key)
	{
		if (key == U"min")
		{
			m_min = 0.0;
			m_value = Max(m_value, m_min);
		}
		else if (key == U"max")
		{
			m_max = 1.0;
			m_value = Max(Min(m_value, m_max), m_min);
		}
		else if (key == U"value")
		{
			// Do nothing
		}
	}

	std::unique_ptr<UIState> VerticalSlider::clone()
	{
		auto ptr = std::make_unique<VerticalSlider>();

		ptr->m_value = m_value;
		ptr->m_min = m_min;
		ptr->m_max = m_max;
		ptr->m_enabled = m_enabled;

		return ptr;
	}
}
