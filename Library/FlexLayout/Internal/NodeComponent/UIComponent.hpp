#pragma once
#include "../../UIState.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	class FlexBoxNode;
}

namespace FlexLayout::Internal::Component
{
	class UIComponent
	{
	public:

		UIComponent(FlexBoxNode& node);

		void copy(const UIComponent& source);

		UIState* state() { return m_state.get(); }

		const UIState* state() const { return m_state.get(); }

		void setState(std::unique_ptr<UIState>&& state);

		void setAdditionalProperty(const StringView key, const StringView value);

		void unsetAdditionalProperty(const StringView key);

		const String& textContent() const { return m_text; }

		void setTextContent(const StringView text);

	private:

		FlexBoxNode& m_node;

		std::unique_ptr<UIState> m_state;

		String m_text;
	};
}
