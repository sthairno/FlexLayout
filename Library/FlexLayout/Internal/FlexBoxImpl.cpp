#include "FlexBoxImpl.hpp"
#include "TreeContext.hpp"
#include "LabelImpl.hpp"
#include "Config.hpp"
#include "../Error.hpp"

namespace FlexLayout::Internal
{
	namespace detail
	{
		void ValidateCircularReference(const FlexBoxImpl* parent, const FlexBoxImpl* child)
		{
			// 自身または祖先を子要素として追加しようとしていないか検証
			for (auto node = parent; node; node = node->parent())
			{
				if (node == child)
				{
					throw InvalidTreeOperationError(U"This operation would create a circular reference\nこの操作によって循環参照が作成されます");
				}
			}
		}

		void ValidateSetChildrenOperation(const FlexBoxImpl* parent, const Array<std::shared_ptr<FlexBoxImpl>>& children)
		{
			for (const auto& child : children)
			{
				ValidateCircularReference(parent, child.get());
			}

			for (auto childItr = children.cbegin(); childItr != children.cend(); childItr++)
			{
				auto& child = *childItr;
				for (auto siblingItr = childItr + 1; siblingItr != children.cend(); siblingItr++)
				{
					auto& sibling = *siblingItr;

					// 重複チェック
					if (child == sibling)
					{
						throw InvalidTreeOperationError(U"Cannot add the same node multiple times\n同じノードを複数回追加することはできません");
					}

					// サブツリー内に別の子要素が存在するかチェック
					if (child->lookupNodeByInstance(*sibling))
					{
						throw InvalidTreeOperationError(U"Duplicated node found in the subtree\nサブツリー内に重複するノードが見つかりました");
					}
				}
			}
		}
	}

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
		detail::ValidateSetChildrenOperation(this, children);

		// 適用

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

		// YGNodeの更新
		YGNodeSetChildren(m_node, ygnodes.data(), ygnodes.size());

		// m_childrenの更新
		m_children = children;
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

		// YGNodeの更新
		YGNodeRemoveAllChildren(m_node);

		// m_childrenの更新
		m_children.clear();
	}

	void FlexBoxImpl::insertChild(std::shared_ptr<FlexBoxImpl> child, size_t index)
	{
		detail::ValidateCircularReference(this, child.get());
		assert(index <= m_children.size());

		// すでにどこかのツリーに所属していた場合は切り離す
		if (child->m_parent)
		{
			child->m_parent->removeChild(child);
		}

		// 子要素の更新
		child->setContext(m_context);
		child->m_parent = this;

		// YGNodeの更新
		YGNodeInsertChild(m_node, child->yogaNode(), index);

		// m_childrenの更新
		m_children.insert(m_children.begin() + index, child);
	}

	void FlexBoxImpl::appendChild(std::shared_ptr<FlexBoxImpl> child)
	{
		insertChild(child, m_children.size());
	}

	void FlexBoxImpl::removeChild(std::shared_ptr<FlexBoxImpl> child)
	{
		auto itr = std::find(m_children.begin(), m_children.end(), child);
		if (itr == m_children.end())
		{
			throw NotFoundError(U"Given node is not a child\n与えられたノードは子ノードではありません");
		}

		// 子要素の更新
		child->m_parent = nullptr;
		child->setContext(nullptr);
		child->clearLayoutOffsetRecursive();

		// YGNodeの更新
		YGNodeRemoveChild(m_node, child->yogaNode());

		// m_childrenの更新
		m_children.erase(itr);
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
