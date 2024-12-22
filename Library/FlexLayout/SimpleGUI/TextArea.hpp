#pragma once
#include "../UIBox.hpp"
#include <Siv3D/SimpleGUI.hpp>

namespace FlexLayout::SimpleGUI
{
	class TextArea : public UIState
	{
	public:

		TextArea();

		TextArea(s3d::StringView text);

	public:

		void attach(UIStateQuery query) override;

		void draw(UIStateQuery query, const Box& box) override;

		void setProperty(UIStateQuery query, s3d::StringView key, s3d::StringView value) override;

		void unsetProperty(UIStateQuery query, s3d::StringView key) override;

		void setTextContent(UIStateQuery query, s3d::StringView text) override;

		std::unique_ptr<UIState> clone() override;

		const s3d::String& text() const
		{
			return m_state->text;
		}

		size_t maxChars() const
		{
			return m_maxChars;
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
			return m_state->textChanged;
		}

	private:

		std::unique_ptr<s3d::TextAreaEditState> m_state;

		size_t m_maxChars;

		bool m_checked = false;

		bool m_enabled = true;
	};
}
