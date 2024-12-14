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
			setTextContent(source.m_text);
		}
	}

	void UIComponent::setState(std::unique_ptr<UIState>&& state)
	{
		assert(not m_state);
		if (m_state == state)
		{
			return;
		}

		m_state = std::move(state);

		m_state->attach(UIStateQuery{ m_node });

		// setState前に追加した設定を追って適用
		for (const auto& [key, value] : m_node.getAdditionalProperties())
		{
			m_state->setProperty(UIStateQuery{ m_node }, key, value);
		}
		if (m_text)
		{
			m_state->setTextContent(UIStateQuery{ m_node }, m_text);
		}
	}

	void UIComponent::setAdditionalProperty(const StringView key, const StringView value)
	{
		if (m_state)
		{
			m_state->setProperty(UIStateQuery{ m_node }, key, value);
		}
	}

	void UIComponent::unsetAdditionalProperty(const StringView key)
	{
		if (m_state)
		{
			m_state->unsetProperty(UIStateQuery{ m_node }, key);
		}
	}

	void UIComponent::setTextContent(const StringView text)
	{
		m_text = text;
		if (m_state)
		{
			m_state->setTextContent(UIStateQuery{ m_node }, text);
		}
	}
}
