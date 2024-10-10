#include "FlexBoxImpl.hpp"
#include "TreeContext.hpp"
#include "LabelImpl.hpp"
#include "Config.hpp"

namespace FlexLayout::Internal
{
	FlexBoxImpl::FlexBoxImpl(const StringView tagName, std::shared_ptr<TreeContext> context)
		: m_node{ GetConfig().createNode() }
		, m_tagName{ tagName }
		, m_context{ context ? context : std::make_shared<TreeContext>() }
	{
		YGNodeSetContext(m_node, this);
	}

	FlexBoxImpl::FlexBoxImpl(const FlexBoxImpl& source, std::shared_ptr<TreeContext> context)
		: FlexBoxImpl(source.m_tagName, context)
	{
		m_propergateOffsetToChildren = source.m_propergateOffsetToChildren;

		copyProperties(source, false, true);
		copyStyles(source);
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
		// TODO: ツリーの変更に応じてTreeContextを更新する

		for (auto& child : m_children)
		{
			child->m_parent = nullptr;
			child->clearLayoutOffsetRecursive();
		}

		Array<YGNodeRef> nodes(Arg::reserve = children.size());
		for (const auto& child : children)
		{
			child->m_parent = this;
			nodes.push_back(child->yogaNode());
		}
		YGNodeSetChildren(m_node, nodes.data(), nodes.size());

		m_children = std::move(children);
	}

	void FlexBoxImpl::removeChildren()
	{
		// TODO: ツリーの変更に応じてTreeContextを更新する

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

	std::shared_ptr<FlexBoxImpl> FlexBoxImpl::clone(std::shared_ptr<TreeContext> context) const
	{
		switch (type())
		{
		case NodeType::Box:
			return std::shared_ptr<FlexBoxImpl>(new FlexBoxImpl(
				*this,
				context
			));
		case NodeType::Label:
			return std::shared_ptr<LabelImpl>(new LabelImpl(
				reinterpret_cast<const LabelImpl&>(*this),
				context
			));
		}

		assert(false && "Unknown NodeType");
		return nullptr;
	}

	std::shared_ptr<FlexBoxImpl> FlexBoxImpl::deepClone(std::shared_ptr<TreeContext> context) const
	{
		if (not context)
		{
			context = std::make_shared<TreeContext>();
		}

		auto newInstance = clone(context);

		Array<std::shared_ptr<FlexBoxImpl>> children(Arg::reserve = m_children.size());
		for (const auto& child : m_children)
		{
			children.push_back(child->deepClone(context));
		}
		newInstance->setChildren(children);

		return newInstance;
	}
}
