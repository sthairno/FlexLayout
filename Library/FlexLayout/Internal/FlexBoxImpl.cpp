#include "FlexBoxImpl.hpp"
#include "TreeContext.hpp"
#include "LabelImpl.hpp"
#include "Config.hpp"

namespace FlexLayout::Internal
{
	FlexBoxImpl::FlexBoxImpl(const StringView tagName)
		: m_node{ GetConfig().createNode() }
		, m_tagName{ tagName }
		, m_computedTextStyle{ GetConfig().defaultTextStyle() }
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

	void FlexBoxImpl::setChildren(const Array<std::shared_ptr<FlexBoxImpl>>& children)
	{
		removeChildren();

		Array<YGNodeRef> ygnodes(Arg::reserve = children.size());
		for (const auto& child : children)
		{
			// すでにどこかに所属していた場合は削除する
			if (child->m_parent)
			{
				child->m_parent->removeChild(child);
			}

			// 子要素の更新
			child->setContext(m_context);
			child->m_parent = this;

			ygnodes.push_back(child->yogaNode());
		}

		// m_childrenの更新
		m_children = std::move(children);

		// YGNodeの更新
		YGNodeSetChildren(m_node, ygnodes.data(), ygnodes.size());
	}

	void FlexBoxImpl::removeChildren()
	{
		// 子要素の更新
		for (auto& child : m_children)
		{
			child->m_parent = nullptr;
			child->setContext(nullptr);
			child->clearLayoutOffsetRecursive();
		}

		// m_childrenの更新
		m_children.clear();

		// YGNodeの更新
		YGNodeRemoveAllChildren(m_node);
	}

	void FlexBoxImpl::insertChild(std::shared_ptr<FlexBoxImpl> child, size_t index)
	{
		assert(index <= m_children.size());

		// すでにどこかに所属していた場合は削除する
		if (child->m_parent)
		{
			child->m_parent->removeChild(child);
		}

		// 子要素の更新
		child->setContext(m_context);
		child->m_parent = this;

		// m_childrenの更新
		m_children.insert(m_children.begin() + index, child);

		// YGNodeの更新
		YGNodeInsertChild(m_node, child->yogaNode(), index);
	}

	void FlexBoxImpl::appendChild(std::shared_ptr<FlexBoxImpl> child)
	{
		insertChild(child, m_children.size());
	}

	void FlexBoxImpl::removeChild(std::shared_ptr<FlexBoxImpl> child)
	{
		assert(m_children.contains(child));

		// 子要素の更新
		child->m_parent = nullptr;
		child->setContext(nullptr);
		child->clearLayoutOffsetRecursive();

		// m_childrenの更新
		m_children.remove(child);

		// YGNodeの更新
		YGNodeRemoveChild(m_node, child->yogaNode());
	}

	TreeContext& FlexBoxImpl::context()
	{
		if (not m_context)
		{
			setContext(std::make_shared<TreeContext>());
		}
		return *m_context;
	}

	void FlexBoxImpl::setContext(std::shared_ptr<TreeContext> context)
	{
		if (m_context == context)
		{
			return;
		}
		getRoot().setContextImpl(context);
	}

	void FlexBoxImpl::setContextImpl(std::shared_ptr<TreeContext> context)
	{
		m_context = context;

		if (m_context)
		{
			context->onNewNodeJoined(shared_from_this());
		}

		for (auto& child : m_children)
		{
			child->setContextImpl(context);
		}
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

	const FlexBoxImpl& FlexBoxImpl::getRoot() const
	{
		auto ptr = this;
		while (ptr->parent())
		{
			ptr = ptr->parent();
		}
		return *ptr;
	}

	std::shared_ptr<FlexBoxImpl> FlexBoxImpl::clone() const
	{
		auto instance = std::make_shared<FlexBoxImpl>(tagName());

		instance->setPropergateOffset(propergateOffset());
		instance->copyProperties(*this, false, true);
		instance->copyStyles(*this);

		return instance;
	}

	std::shared_ptr<FlexBoxImpl> FlexBoxImpl::deepClone() const
	{
		auto newInstance = clone();

		Array<std::shared_ptr<FlexBoxImpl>> children(Arg::reserve = m_children.size());
		for (const auto& child : m_children)
		{
			children.push_back(child->deepClone());
		}
		newInstance->setChildren(children);

		return newInstance;
	}

	bool FlexBoxImpl::BelongsToSameTree(const FlexBoxImpl& a, const FlexBoxImpl& b)
	{
		if (a.m_context == b.m_context)
		{
			return true;
		}

		if (a.m_context)
		{
			return true;
		}

		// contextが初期化されておらず判定できない場合はルート要素を比較する
		return &a.getRoot() == &b.getRoot();
	}
}
