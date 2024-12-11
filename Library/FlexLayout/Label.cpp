#include "Label.hpp"
#include "Internal/FlexBoxNode.hpp"
#include "Internal/NodeComponent/TextComponent.hpp"

namespace FlexLayout
{
	String Label::text() const
	{
		return String{ m_node->getComponent<Internal::Component::TextComponent>().text() };
	}

	void Label::setText(const StringView text)
	{
		m_node->getComponent<Internal::Component::TextComponent>()
			.setText(text);
	}

	void Label::draw(const ColorF& color) const
	{
		m_node->getComponent<Internal::Component::TextComponent>()
			.draw(TextStyle::Default(), color);
	}

	void Label::draw(const TextStyle& textStyle, const ColorF& color) const
	{
		m_node->getComponent<Internal::Component::TextComponent>()
			.draw(textStyle, color);
	}
}
