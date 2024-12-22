#pragma once
#include "../UIBox.hpp"
#include <Siv3D/SimpleGUI.hpp>

namespace FlexLayout::SimpleGUI
{
	class HorizontalRadioButtons : public UIState
	{
	public:

		void attach(UIStateQuery query) override;

		void draw(UIStateQuery query, const Box& box) override;

		void setTextContent(UIStateQuery query, s3d::StringView text) override;

		std::unique_ptr<UIState> clone() override;

		const auto& options() const
		{
			return m_options;
		}

		size_t index() const
		{
			return m_index;
		}

		void setIndex(size_t index)
		{
			assert(index < m_options.size());
			m_changed |= m_index != index;
			m_index = index;
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

		s3d::Array<s3d::String> m_options{ U"" };

		size_t m_index = 0;

		bool m_enabled = true;

		bool m_changed = false;
	};
}
