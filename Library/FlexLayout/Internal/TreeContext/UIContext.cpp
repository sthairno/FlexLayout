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
			node.getComponent<Component::UIComponent>()
				.update();
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
			node.getComponent<Component::UIComponent>()
				.draw();
		}

		for (const auto& child : node.children())
		{
			draw(*child);
		}
	}
}
