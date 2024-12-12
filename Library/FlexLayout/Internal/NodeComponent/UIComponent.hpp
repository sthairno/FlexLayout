#pragma once
#include "../../UIState.hpp"
#include <memory>

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

		void removeState();

	private:

		FlexBoxNode& m_node;

		std::unique_ptr<UIState> m_state;
	};
}
