#include "UIComponent.hpp"
#include "../../UIState.hpp"
#include "../FlexBoxNode.hpp"

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
		if (m_state == state)
		{
			return;
		}

		if (m_state)
		{
			m_state->detach(UIStateQuery{ m_node });
		}

		m_state = std::move(state);

		m_state->attach(UIStateQuery{ m_node });
		for (const auto& [key, value] : m_node.getAdditionalProperties())
		{
			m_state->setProperty(key, value);
		}
	}

	void UIComponent::removeState()
	{
		if (m_state)
		{
			m_state->detach(UIStateQuery{ m_node });
			m_state.reset();
		}
	}

	void UIComponent::setAdditionalProperty(const StringView key, const StringView value)
	{
		if (m_state)
		{
			m_state->setProperty(key, value);
		}
	}

	void UIComponent::unsetAdditionalProperty(const StringView key)
	{
		if (m_state)
		{
			m_state->unsetProperty(key);
		}
	}
}
