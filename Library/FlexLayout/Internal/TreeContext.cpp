#include "TreeContext.hpp"
#include "FlexBoxNode.hpp"

namespace FlexLayout::Internal
{
	void TreeContext::onNewNodeJoin(const std::shared_ptr<FlexBoxNode>& node)
	{
		getContext<Context::StyleContext>()
			.queueStyleApplication(node);
	}
}
