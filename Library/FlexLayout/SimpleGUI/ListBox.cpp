#include "ListBox.hpp"

using namespace s3d;

namespace FlexLayout::SimpleGUI
{
	void ListBox::attach(UIStateQuery query)
	{
		auto minSize = s3d::SimpleGUI::ListBoxRegion(
			{ 0, 0 }, 0, 0
		).size;

		query.setStyle(
			U"min-width",
			StyleValue::Length(static_cast<float>(minSize.x), LengthUnit::Pixel)
		);
		query.setStyle(
			U"min-height",
			StyleValue::Length(static_cast<float>(minSize.y), LengthUnit::Pixel)
		);
	}

	void ListBox::draw(UIStateQuery, const Box& box)
	{
		m_changed = false;
		if (auto rect = box.contentAreaRect())
		{
			m_changed = s3d::SimpleGUI::ListBox(
				m_state,
				rect->pos,
				rect->w,
				rect->h,
				m_enabled
			);
		}
	}

	void ListBox::setTextContent(UIStateQuery, s3d::StringView text)
	{
		if (text.isEmpty())
		{
			m_state.items.clear();
		}
		else
		{
			m_state.items = String{ text }.split_lines();
		}

		if (m_state.selectedItemIndex.has_value() &&
			m_state.selectedItemIndex >= m_state.items.size())
		{
			m_state.selectedItemIndex.reset();
		}
	}

	std::unique_ptr<UIState> ListBox::clone()
	{
		auto ptr = std::make_unique<ListBox>();

		ptr->m_state = ListBoxState{
			m_state.items,
		};
		ptr->m_enabled = m_enabled;

		return ptr;
	}
}
