﻿#include "StyleContext.hpp"
#include "../FlexBoxNode.hpp"
#include "../NodeComponent/StyleComponent.hpp"

namespace FlexLayout::Internal::Context
{
	void StyleContext::applyStyles(FlexBoxNode& root)
	{
		auto& context = root.context();

		if (m_styleApplicationWaitinglist.empty())
		{
			return;
		}

		// 浅い順にソートしなおす
		using _QueueValue = std::pair<size_t, std::shared_ptr<FlexBoxNode>>;
		using _QueueComp = decltype([](const _QueueValue& a, const _QueueValue& b) { return a.first > b.first; });
		using _QueueType = std::priority_queue<_QueueValue, Array<_QueueValue>, _QueueComp>;
		_QueueType queue;
		for (const auto& weakptr : m_styleApplicationWaitinglist)
		{
			if (auto item = weakptr.lock();
				item && FlexBoxNode::BelongsToSameTree(root, *item))
			{
				queue.push({ item->getDepth(), item });
			}
		}
		m_styleApplicationWaitinglist.clear();

		// 適用
		while (not queue.empty())
		{
			auto& [depth, item] = queue.top();
			auto& component = item->getComponent<Component::StyleComponent>();
			if (component.isStyleApplicationScheduled())
			{
				component.applyStylesImpl();
			}
			queue.pop();
		}
	}

	void StyleContext::queueStyleApplication(const std::shared_ptr<FlexBoxNode>& node)
	{
		m_styleApplicationWaitinglist.push_back(node);
	}
}
