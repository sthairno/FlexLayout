#include "FlexBoxNode.hpp"
#include "TreeContext.hpp"
#include "Config.hpp"
#include "../Error.hpp"

#include "NodeComponent/LayoutComponent.hpp"
#include "NodeComponent/StyleComponent.hpp"
#include "NodeComponent/XmlAttributeComponent.hpp"
#include "NodeComponent/TextComponent.hpp"

namespace FlexLayout::Internal
{
	namespace detail
	{
		static void ValidateCircularReference(const FlexBoxNode* parent, const FlexBoxNode* child)
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

		static bool LookupNodeFromChildren(const FlexBoxNode* node, const FlexBoxNode* target)
		{
			for (const auto& child : node->children())
			{
				if (child.get() == target ||
					LookupNodeFromChildren(child.get(), target))
				{
					return true;
				}
			}

			return false;
		}

		static void ValidateSetChildrenOperation(const FlexBoxNode* parent, const Array<std::shared_ptr<FlexBoxNode>>& children)
		{
			// 既存ツリーとの重複チェック
			for (const auto& child : children)
			{
				ValidateCircularReference(parent, child.get());
			}

			// children内の重複チェック
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
					if (LookupNodeFromChildren(child.get(), sibling.get()))
					{
						throw InvalidTreeOperationError(U"Duplicated node found in the subtree\nサブツリー内に重複するノードが見つかりました");
					}
				}
			}
		}
	}

	FlexBoxNode::FlexBoxNode(FlexBoxNodeOptions options)
		: m_yogaNode{ GetConfig().createNode() }
		, m_components{
			std::make_unique<Component::LayoutComponent>(*this),
			std::make_unique<Component::StyleComponent>(*this),
			std::make_unique<Component::XmlAttributeComponent>(*this),
			options.textNode
				? std::make_unique<Component::TextComponent>(*this)
				: std::unique_ptr<Component::TextComponent>{}
		}
	{
		YGNodeSetContext(m_yogaNode, this);
	}

	FlexBoxNode::~FlexBoxNode()
	{
		// 先に子要素のデストラクタを呼び出させる
		removeChildren();

		// YGNodeを解放
		YGNodeFree(m_yogaNode);
		m_yogaNode = nullptr;
	}

	void FlexBoxNode::setChildren(const Array<std::shared_ptr<FlexBoxNode>>& children)
	{
		assert(not isTextNode());
		assert(children.all([](const auto& child) { return !!child; }));
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
		YGNodeSetChildren(m_yogaNode, ygnodes.data(), ygnodes.size());

		// m_childrenの更新
		m_children = children;
	}

	void FlexBoxNode::removeChildren()
	{
		// 子要素の更新
		for (auto& child : m_children)
		{
			child->m_parent = nullptr;
			child->setContext(nullptr);
			child->getComponent<Component::LayoutComponent>()
				.clearLayoutOffsetRecursive();
		}

		// YGNodeの更新
		YGNodeRemoveAllChildren(m_yogaNode);

		// m_childrenの更新
		m_children.clear();
	}

	void FlexBoxNode::insertChild(const std::shared_ptr<FlexBoxNode>& child, size_t index)
	{
		assert(not isTextNode());
		assert(child);
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
		YGNodeInsertChild(m_yogaNode, child->yogaNode(), index);

		// m_childrenの更新
		m_children.insert(m_children.begin() + index, child);
	}

	void FlexBoxNode::appendChild(const std::shared_ptr<FlexBoxNode>& child)
	{
		insertChild(child, m_children.size());
	}

	void FlexBoxNode::removeChild(const std::shared_ptr<FlexBoxNode>& child)
	{
		assert(child);

		auto itr = std::find(m_children.begin(), m_children.end(), child);
		if (itr == m_children.end())
		{
			throw NotFoundError(U"Given node is not a child\n与えられたノードは子ノードではありません");
		}

		// 子要素の更新
		child->m_parent = nullptr;
		child->setContext(nullptr);
		child->getComponent<Component::LayoutComponent>()
			.clearLayoutOffsetRecursive();

		// YGNodeの更新
		YGNodeRemoveChild(m_yogaNode, child->yogaNode());

		// m_childrenの更新
		m_children.erase(itr);
	}

	TreeContext& FlexBoxNode::context()
	{
		if (not m_context)
		{
			setContext(std::make_shared<TreeContext>());
		}
		return *m_context;
	}

	void FlexBoxNode::setContext(const std::shared_ptr<TreeContext>& context)
	{
		if (m_context == context)
		{
			return;
		}
		getRoot().setContextImpl(context);
	}

	void FlexBoxNode::setContextImpl(const std::shared_ptr<TreeContext>& context)
	{
		m_context = context;

		if (m_context)
		{
			context->onNewNodeJoin(shared_from_this());
		}

		for (auto& child : m_children)
		{
			child->setContextImpl(context);
		}
	}

	size_t FlexBoxNode::getDepth() const
	{
		size_t depth = 0;
		for (const FlexBoxNode* item = parent(); item; item = item->parent())
		{
			depth++;
		}
		return depth;
	}

	FlexBoxNode& FlexBoxNode::getRoot()
	{
		auto ptr = this;
		while (ptr->parent())
		{
			ptr = ptr->parent();
		}
		return *ptr;
	}

	const FlexBoxNode& FlexBoxNode::getRoot() const
	{
		auto ptr = this;
		while (ptr->parent())
		{
			ptr = ptr->parent();
		}
		return *ptr;
	}

	std::shared_ptr<FlexBoxNode> FlexBoxNode::clone() const
	{
		auto instance = std::make_shared<FlexBoxNode>(FlexBoxNodeOptions{
			.textNode = isTextNode()
		});

		instance->getComponent<Component::LayoutComponent>()
			.copy(getComponent<Component::LayoutComponent>());
		instance->getComponent<Component::StyleComponent>()
			.copy(getComponent<Component::StyleComponent>());
		instance->getComponent<Component::XmlAttributeComponent>()
			.copy(getComponent<Component::XmlAttributeComponent>());
		if (isTextNode())
		{
			instance->getComponent<Component::TextComponent>()
				.copy(getComponent<Component::TextComponent>());
		}

		instance->m_additonalProperties = m_additonalProperties;

		return instance;
	}

	std::shared_ptr<FlexBoxNode> FlexBoxNode::deepClone() const
	{
		auto newInstance = clone();

		if (m_children)
		{
			Array<std::shared_ptr<FlexBoxNode>> children(Arg::reserve = m_children.size());
			for (const auto& child : m_children)
			{
				children.push_back(child->deepClone());
			}
			newInstance->setChildren(children);
		}

		return newInstance;
	}

	bool FlexBoxNode::isTextNode() const
	{
		return !!std::get<std::unique_ptr<Component::TextComponent>>(m_components);
	}

	bool FlexBoxNode::BelongsToSameTree(const FlexBoxNode& a, const FlexBoxNode& b)
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

	Optional<String> FlexBoxNode::getProperty(const StringView key) const
	{
		if (key == U"id")
		{
			return getComponent<Component::XmlAttributeComponent>().id();
		}
		else if (key == U"class")
		{
			return getComponent<Component::XmlAttributeComponent>().getClassText();
		}
		else if (key == U"style")
		{
			return getComponent<Component::StyleComponent>().getInlineCssText();
		}
		else if (key == U"siv3d-font")
		{
			return getComponent<Component::StyleComponent>().fontId();
		}
		else if (const auto it = m_additonalProperties.find(key);
			it != m_additonalProperties.end())
		{
			return it->second;
		}

		return none;
	}

	void FlexBoxNode::setProperty(const StringView key, const StringView value)
	{
		if (key == U"id")
		{
			getComponent<Component::XmlAttributeComponent>().setId(MakeOptional<String>(value));
		}
		else if (key == U"class")
		{
			getComponent<Component::XmlAttributeComponent>().setClassText(value);
		}
		else if (key == U"style")
		{
			getComponent<Component::StyleComponent>().setInlineCssText(value);
		}
		else if (key == U"siv3d-font")
		{
			getComponent<Component::StyleComponent>().setFont(value);
		}
		else
		{
			m_additonalProperties[key] = value;
		}
	}

	bool FlexBoxNode::removeProperty(const StringView key)
	{
		if (key == U"id")
		{
			getComponent<Component::XmlAttributeComponent>().setId(none);
		}
		else if (key == U"class")
		{
			getComponent<Component::XmlAttributeComponent>().clearClasses();
		}
		else if (key == U"style")
		{
			getComponent<Component::StyleComponent>().clearStyles(StylePropertyGroup::Inline);
		}
		else if (key == U"siv3d-font")
		{
			getComponent<Component::StyleComponent>().setFont({ }, U"");
		}

		return m_additonalProperties.erase(key);
	}

	void FlexBoxNode::clearProperties()
	{
		auto& xmlAttr = getComponent<Component::XmlAttributeComponent>();
		auto& style = getComponent<Component::StyleComponent>();

		xmlAttr.setId(none);
		xmlAttr.clearClasses();
		style.clearStyles(StylePropertyGroup::Inline);
		style.setFont({ }, U"");

		m_additonalProperties.clear();
	}

	void CalculateLayout(FlexBoxNode& node, Optional<float> width, Optional<float> height)
	{
		YGNodeCalculateLayout(
			node.yogaNode(),
			width.value_or(YGUndefined),
			height.value_or(YGUndefined),
			YGDirectionLTR);
	}
}
