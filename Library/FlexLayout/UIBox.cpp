#include "UIBox.hpp"
#include "Internal/FlexBoxNode.hpp"
#include "Internal/NodeComponent/UIComponent.hpp"

namespace FlexLayout::detail
{
	UIState* UIBoxBase::getState()
	{
		auto ptr = m_node
			->getComponent<Internal::Component::UIComponent>()
			.state();

		assert(ptr);

		return ptr;
	}

	const UIState* UIBoxBase::getState() const
	{
		auto ptr = m_node
			->getComponent<Internal::Component::UIComponent>()
			.state();

		assert(ptr);

		return ptr;
	}
}
