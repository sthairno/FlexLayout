#include "UIComponent.hpp"
#include "../../UIState.hpp"

namespace FlexLayout::Internal::Component
{
	UIComponent::UIComponent(FlexBoxNode& node)
		: m_node(node)
	{ }

	void UIComponent::copy(const UIComponent& source)
	{
		if (source.m_state)
		{
			setState(source.m_state->clone());
		}
		else
		{
			removeState();
		}
	}

	void UIComponent::setState(std::unique_ptr<UIState>&& state)
	{
		if (m_state)
		{
			m_state->detach(UIStateQuery{ m_node });
		}

		m_state = std::move(state);
		m_state->attach(UIStateQuery{ m_node });
	}

	void UIComponent::removeState()
	{
		if (m_state)
		{
			m_state->detach(UIStateQuery{ m_node });
			m_state.reset();
		}
	}
}
