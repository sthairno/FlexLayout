#pragma once
#include "../UIBox.hpp"
#include <Siv3D/SimpleGUI.hpp>

namespace FlexLayout::SimpleGUI
{
	class ListBox : public UIState
	{
	public:

		void attach(UIStateQuery query) override;

		void draw(UIStateQuery query, const Box& box) override;

		void setTextContent(UIStateQuery query, s3d::StringView text) override;

		std::unique_ptr<UIState> clone() override;

		const s3d::Array<s3d::String>& items() const
		{
			return m_state.items;
		}

		s3d::Optional<size_t> selectedItemIndex() const
		{
			return m_state.selectedItemIndex;
		}

		s3d::Optional<s3d::StringView> selectedItem() const
		{
			if (m_state.selectedItemIndex.has_value())
			{
				return m_state.items[*m_state.selectedItemIndex];
			}
			else
			{
				return s3d::none;
			}
		}

		void selectItem(size_t itemIndex)
		{
			assert(itemIndex < m_state.items.size());
			m_state.selectedItemIndex = itemIndex;
		}

		void clearSelection()
		{
			m_state.selectedItemIndex.reset();
		}

		bool isScrollBarGrabbed() const
		{
			return m_state.scrollBarGrabbed.has_value();
		}

		s3d::int32 scroll() const
		{
			return m_state.scroll;
		}

		bool active() const
		{
			return m_state.active;
		}

		bool enabled() const
		{
			return m_enabled;
		}

		void setEnabled(bool enabled)
		{
			m_enabled = enabled;
		}

		bool changed() const
		{
			return m_changed;
		}

	private:

		s3d::ListBoxState m_state;

		bool m_enabled = true;

		bool m_changed = false;
	};
}
