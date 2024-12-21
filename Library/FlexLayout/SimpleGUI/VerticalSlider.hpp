#pragma once
#include "../UIBox.hpp"
#include <Siv3D/SimpleGUI.hpp>

namespace FlexLayout::SimpleGUI
{
	class VerticalSlider : public UIState
	{
	public:

		void attach(UIStateQuery query) override;

		void draw(UIStateQuery query, const Box& box) override;

		void setProperty(UIStateQuery query, s3d::StringView key, s3d::StringView value) override;

		void unsetProperty(UIStateQuery query, s3d::StringView key) override;

		std::unique_ptr<UIState> clone() override;

		double value() const
		{
			return m_value;
		}

		void setValue(double value)
		{
			m_changed |= m_value != value;
			m_value = value;
		}

		double min() const
		{
			return m_min;
		}

		double max() const
		{
			return m_max;
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

		double m_value = 0.0;

		double m_min = 0.0;

		double m_max = 0.0;

		bool m_enabled = true;

		bool m_changed = false;
	};
}
