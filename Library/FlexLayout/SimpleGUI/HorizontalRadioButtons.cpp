#include "HorizontalRadioButtons.hpp"

using namespace s3d;

namespace FlexLayout::SimpleGUI
{
	void HorizontalRadioButtons::attach(UIStateQuery query)
	{
		auto size = s3d::SimpleGUI::HorizontalRadioButtonsRegion(
			m_options,
			{ 0, 0 }
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

	void HorizontalRadioButtons::draw(UIStateQuery, const Box& box)
	{
		m_changed = false;
		if (auto rect = box.contentAreaRect())
		{
			m_changed = s3d::SimpleGUI::HorizontalRadioButtons(
				m_index,
				m_options,
				rect->pos,
				rect->w / m_options.size(),
				m_enabled
			);
		}
	}

	void HorizontalRadioButtons::setTextContent(UIStateQuery query, s3d::StringView text)
	{
		m_options = text.isEmpty()
			? Array<String>{ U"" } : String{ text }.split_lines();

		if (m_index >= m_options.size())
		{
			m_index = 0;
		}

		auto size = s3d::SimpleGUI::HorizontalRadioButtonsRegion(
			m_options,
			{ 0, 0 }
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

	std::unique_ptr<UIState> HorizontalRadioButtons::clone()
	{
		auto ptr = std::make_unique<HorizontalRadioButtons>();

		ptr->m_options = m_options;
		ptr->m_index = m_index;
		ptr->m_enabled = m_enabled;

		return ptr;
	}
}
