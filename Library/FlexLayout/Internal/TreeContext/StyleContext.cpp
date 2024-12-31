#include "StyleContext.hpp"
#include "../FlexBoxNode.hpp"
#include "../NodeComponent/StyleComponent.hpp"

namespace FlexLayout::Internal::Context
{
	void StyleContext::applyProperties(FlexBoxNode& root)
	{
		auto& styleComponent = root.getComponent<Component::StyleComponent>();
		if (styleComponent.needApplyProperties())
		{
			detail::PropertyApplicationState state;
			styleComponent.applyProperties(state);
		}
	}
}
