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

	FlexBoxImpl::FlexBoxImpl(const FlexBoxImpl& source)
		: FlexBoxImpl(source.m_tagName)
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
		removeChildren();

		Array<YGNodeRef> nodes(Arg::reserve = children.size());
		for (const auto& child : children)
		{
			child->m_parent = this;
			child->setContext(m_context);
			nodes.push_back(child->yogaNode());
		}
		YGNodeSetChildren(m_node, nodes.data(), nodes.size());

		m_children = std::move(children);
	}

	void FlexBoxImpl::removeChildren()
	{
		// YGNodeFreeでは親要素の配列から子要素を削除し、後続の子要素を詰める処理が走る。
		// (参考：erase-removeイディオム)
		// そのため、デストラクタでYGNodeFreeを呼び出することを考慮して逆順に解放する

		for (auto itr = m_children.rbegin(); itr != m_children.rend(); itr++)
		{
			auto& child = *itr;

			child->m_parent = nullptr;
			child->setContext(nullptr);
			child->clearLayoutOffsetRecursive();

			child.reset();
		}
		m_children.clear();
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
		switch (type())
		{
		case NodeType::Box:
			return std::shared_ptr<FlexBoxImpl>(new FlexBoxImpl(
				*this
			));
		case NodeType::Label:
			return std::shared_ptr<LabelImpl>(new LabelImpl(
				reinterpret_cast<const LabelImpl&>(*this)
			));
		}

		assert(false && "Unknown NodeType");
		return nullptr;
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
