#pragma once
#include "../UIBox.hpp"
#include <Siv3D/SimpleGUI.hpp>

namespace FlexLayout::SimpleGUI
{
	class Button : public UIState
	{
	public:

		void attach(UIStateQuery query) override;

		void draw(UIStateQuery query, const Box& box) override;

		void setTextContent(UIStateQuery query, s3d::StringView text) override;

		std::unique_ptr<UIState> clone() override;

		bool clicked() const
		{
			return m_clicked;
		}

		void setEnabled(bool enabled)
		{
			m_enabled = enabled;
			m_clicked &= enabled;
		}

		bool enabled() const
		{
			return m_enabled;
		}

	private:

		bool m_clicked = false;

		bool m_enabled = true;
	};
}
