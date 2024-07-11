#include "FlexBoxImpl.hpp"
#include "../Util/ParseInlineCSS.hpp"
#include "../Util/LoadStyleToYogaNode.hpp"

namespace FlexLayout::detail
{
	FlexBoxImpl::FlexBoxImpl(std::shared_ptr<TreeContext> context)
		: m_context{ context }
		, m_node{ context->createNode() }
	{
		YGNodeSetContext(m_node, this);
	}

	void FlexBoxImpl::reset()
	{
		removeChildren();
		clearProperties();
		YGNodeReset(m_node);
	}

	bool FlexBoxImpl::addClass(const StringView className)
	{
		String str{ className };
		str.trim();

		if (str.isEmpty())
		{
			return false;
		}
		if (str.contains(U' '))
		{
			throw Error{ U"FlexBox: Class name should not contain space" };
		}

		if (m_classes.contains(str))
		{
			return false;
		}
		else
		{
			m_classes.emplace_back(std::move(str));
			return true;
		}
	}

	bool FlexBoxImpl::removeClass(const StringView className)
	{
		String str{ className };
		str.trim();

		if (str.isEmpty())
		{
			return false;
		}
		if (str.contains(U' '))
		{
			throw Error{ U"FlexBox: Class name should not contain space" };
		}

		auto prevSize = m_classes.size();
		m_classes.remove(str);
		return m_classes.size() != prevSize;
	}

	Optional<String> FlexBoxImpl::getProperty(const StringView key) const
	{
		if (const auto it = m_properties.find(key);
			it != m_properties.end())
		{
			return it->second;
		}
		else
		{
			return none;
		}
	}

	void FlexBoxImpl::clearProperties()
	{
		m_id.reset();
		m_classes.clear();
		m_properties.clear();
		m_labelRenderer.reset();
		resetStyle();
	}

	void FlexBoxImpl::setProperty(const StringView key, const StringView value)
	{
		if (key == U"id")
		{
			m_id = value;
		}
		else if (key == U"class")
		{
			m_classes.clear();
			for (auto& className : String{ value }.split(U' '))
			{
				className = className.trimmed();
				if (className.isEmpty())
				{
					continue;
				}
				if (not m_classes.contains(className))
				{
					m_classes.emplace_back(std::move(className));
				}
			}
		}
		else if (key == U"style")
		{
			loadStyle(String{ value });
		}

		m_properties[key] = value;
	}

	void FlexBoxImpl::removeProperty(const StringView key)
	{
		if (key == U"id")
		{
			m_id.reset();
		}
		else if (key == U"class")
		{
			m_classes.clear();
		}
		else if (key == U"style")
		{
			resetStyle();
		}

		m_properties.erase(key);
	}

	void FlexBoxImpl::removeChildren()
	{
		for (auto itr = m_children.rbegin(); itr != m_children.rend(); itr++)
		{
			(*itr)->resetLayoutOffset();
			itr->reset();
		}
		m_children.clear();
	}

	void FlexBoxImpl::lookupNodesByClassName(Array<std::shared_ptr<FlexBoxImpl>>& list, const String& className, size_t limit)
	{
		if (list.size() >= limit)
		{
			return;
		}

		if (m_classes.contains(className))
		{
			list.push_back(shared_from_this());
		}

		for (const auto& child : m_children)
		{
			child->lookupNodesByClassName(list, className, limit);
		}
	}

	std::shared_ptr<FlexBoxImpl> FlexBoxImpl::lookupNodeById(const StringView id)
	{
		if (m_id == id)
		{
			return shared_from_this();
		}

		for (const auto& child : m_children)
		{
			if (auto node = child->lookupNodeById(id))
			{
				return node;
			}
		}

		return { };
	}

	void FlexBoxImpl::setLayoutOffsetRecursive(Optional<Vec2> offset, bool force)
	{
		// 更新部分だけ処理する
		// https://www.yogalayout.dev/docs/advanced/incremental-layout
		if (not force && offset == m_layoutOffset)
		{
			if (not YGNodeGetHasNewLayout(m_node))
			{
				return;
			}
		}
		YGNodeSetHasNewLayout(m_node, false);

		Optional<Vec2> childOffset;
		if (offset && YGNodeStyleGetDisplay(m_node) != YGDisplayNone)
		{
			m_layoutOffset = *offset;
			childOffset = *offset + Vec2{ YGNodeLayoutGetLeft(m_node), YGNodeLayoutGetTop(m_node) };
		}
		else
		{
			m_layoutOffset.reset();
		}

		for (const auto& child : m_children)
		{
			child->setLayoutOffsetRecursive(childOffset);
		}
	}

	void FlexBoxImpl::resetLayoutOffset()
	{
		m_layoutOffset.reset();
		for (const auto& child : m_children)
		{
			child->resetLayoutOffset();
		}
	}

	void FlexBoxImpl::setChildren(const Array<std::shared_ptr<FlexBoxImpl>>& children)
	{
		for (auto& child : m_children)
		{
			child->resetLayoutOffset();
		}

		Array<YGNodeRef> nodes(Arg::reserve = children.size());
		for (const auto& child : children)
		{
			nodes.push_back(child->yogaNode());
		}
		YGNodeSetChildren(m_node, nodes.data(), nodes.size());

		m_children = std::move(children);
	}

	void FlexBoxImpl::resetStyle()
	{
		m_context->resetNodeStyle(m_node);
		m_labelProperty = {};
	}

	void FlexBoxImpl::loadStyle(const String& css)
	{
		for (const auto& [key, value] : Util::ParseInlineCSS(css))
		{
			if (Util::LoadStyleToYogaNode(m_node, key, value))
			{
				// YGNodeMarkDirty(m_node);
			}
			else if (LoadStyleToLabelProperty(m_labelProperty, key, value))
			{
				// YGNodeMarkDirty(m_node);
			}
		}
	}

	FlexBoxImpl::~FlexBoxImpl()
	{
		// 先に子要素のデストラクタを呼び出させる
		removeChildren();

		// YGNodeを解放
		YGNodeFree(m_node);
		m_node = nullptr;
	}
}
