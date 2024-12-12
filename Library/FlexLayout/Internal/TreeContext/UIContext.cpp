#include "UIContext.hpp"

#include "../FlexBoxNode.hpp"
#include "../../Box.hpp"
#include "../NodeComponent/UIComponent.hpp"
#include "../NodeComponent/TextComponent.hpp"

namespace FlexLayout::Internal::Context
{
	void UIContext::update(FlexBoxNode& node)
	{
		for (auto itr = node.children().rbegin();
			itr != node.children().rend();
			itr++)
		{
			update(**itr);
		}

		if (node.isUINode())
		{
			auto& component = node.getComponent<Component::UIComponent>();
			if (component.state())
			{
				component.state()->update(Box{ node.shared_from_this() });
			}
		}
	}

	void UIContext::draw(FlexBoxNode& node)
	{
		if (node.isTextNode())
		{
			auto& component = node.getComponent<Component::TextComponent>();
			component.draw(TextStyle::Default(), Palette::White);
		}

		if (node.isUINode())
		{
			auto& component = node.getComponent<Component::UIComponent>();
			if (component.state())
			{
				component.state()->draw(Box{ node.shared_from_this() });
			}
		}

		for (const auto& child : node.children())
		{
			draw(*child);
		}
	}
}
