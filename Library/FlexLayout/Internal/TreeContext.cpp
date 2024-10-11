#include "TreeContext.hpp"
#include "FlexBoxImpl.hpp"

namespace FlexLayout::Internal
{
	void TreeContext::queueStyleApplication(std::shared_ptr<FlexBoxImpl> node)
	{
		m_styleApplicationWaitinglist.push_back(node);
	}

	void TreeContext::onNewNodeJoined(std::shared_ptr<FlexBoxImpl> node)
	{
		if (node->isStyleApplicationScheduled())
		{
			queueStyleApplication(node);
		}
	}
}
