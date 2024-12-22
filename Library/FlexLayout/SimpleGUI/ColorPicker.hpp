#pragma once
#include "../UIBox.hpp"
#include <Siv3D/SimpleGUI.hpp>

namespace FlexLayout::SimpleGUI
{
	class ColorPicker : public UIState
	{
	public:

		void attach(UIStateQuery query) override;

		void draw(UIStateQuery query, const Box& box) override;

		std::unique_ptr<UIState> clone() override;

		s3d::HSV value() const
		{
			return m_value;
		}

		void setValue(s3d::HSV value)
		{
			m_changed |= m_value != value;
			m_value = value;
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

		s3d::HSV m_value;

		bool m_enabled = true;

		bool m_changed = false;
	};
}
