#include "FlexBoxImpl.hpp"
#include "TreeContext.hpp"

namespace FlexLayout::Internal
{
	FlexBoxImpl::FlexBoxImpl(std::shared_ptr<TreeContext> context, const StringView tagName)
		: m_node{ context->createNode() }
		, m_tagName{ tagName }
		, m_context{ context }
	{
		YGNodeSetContext(m_node, this);
	}

	FlexBoxImpl::~FlexBoxImpl()
	{
		// 先に子要素のデストラクタを呼び出させる
		removeChildren();

		// YGNodeを解放
		YGNodeFree(m_node);
		m_node = nullptr;
	}

	void FlexBoxImpl::setChildren(Array<std::shared_ptr<FlexBoxImpl>>& children)
	{
		for (auto& child : m_children)
		{
			child->m_parent = nullptr;
			child->clearLayoutOffsetRecursive();
		}

		Array<YGNodeRef> nodes(Arg::reserve = children.size());
		for (const auto& child : children)
		{
			assert(m_context == child->m_context);
			child->m_parent = this;
			nodes.push_back(child->yogaNode());
		}
		YGNodeSetChildren(m_node, nodes.data(), nodes.size());

		m_children = std::move(children);
	}

	void FlexBoxImpl::removeChildren()
	{
		for (auto itr = m_children.rbegin(); itr != m_children.rend(); itr++)
		{
			auto& child = *itr;
			child->m_parent = nullptr;
			child->clearLayoutOffsetRecursive();
			child.reset();
		}
		m_children.clear();
	}

	size_t FlexBoxImpl::getDepth() const
	{
		size_t depth = 0;
		for (const FlexBoxImpl* item = parent(); item; item = item->parent())
		{
			depth++;
		}
		return depth;
	}

	FlexBoxImpl& FlexBoxImpl::getRoot()
	{
		auto ptr = this;
		while (ptr->parent())
		{
			ptr = ptr->parent();
		}
		return *ptr;
	}
}
