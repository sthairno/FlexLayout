#pragma once
#include "../UIBox.hpp"
#include <Siv3D/SimpleGUI.hpp>

namespace FlexLayout::SimpleGUI
{
	class CheckBox : public UIState
	{
	public:

		void attach(UIStateQuery query) override;

		void draw(UIStateQuery query, const Box& box) override;

		void setTextContent(UIStateQuery query, s3d::StringView text) override;

		std::unique_ptr<UIState> clone() override;

		bool checked() const
		{
			return m_checked;
		}

		void setChecked(bool checked)
		{
			m_changed |= m_checked != checked;
			m_checked = checked;
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

		bool m_checked = false;

		bool m_enabled = true;

		bool m_changed = false;
	};
}
