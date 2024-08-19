#include "FlexBoxImpl.hpp"
#include "../Util/ParseInlineCSS.hpp"
#include "../Util/LoadStyleToYogaNode.hpp"

namespace FlexLayout::Internal
{
	FlexBoxImpl::FlexBoxImpl(std::shared_ptr<TreeContext> context, const StringView tagName)
		: m_node{ context->createNode() }
		, m_tagName{ tagName }
		, m_context{ context }
	{
		YGNodeSetContext(m_node, this);
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

	bool FlexBoxImpl::setStyle(const StringView styleName, const StringView value)
	{
		String key{ styleName };
		if (key.trim().isEmpty())
		{
			return false;
		}

		//m_style.emplace(key, value);
	}

	bool FlexBoxImpl::setStyle(const StringView styleName, const float value)
	{
		String key{ styleName };
		if (key.trim().isEmpty())
		{
			return false;
		}

		//m_style.emplace(key, value);
	}

	bool FlexBoxImpl::removeStyle(const StringView styleName)
	{
		if (const auto it = m_style.find(styleName);
			it != m_style.end())
		{
			m_style.erase(it);
			return true;
		}

		return false;
	}

	Optional<String> FlexBoxImpl::getProperty(const StringView key) const
	{
		if (key == U"id")
		{
			return m_id;
		}
		else if (key == U"class")
		{
			return m_classes.join(U" ", U"", U"");
		}
		else if (key == U"style")
		{
			Array<std::pair<String, String>> tmp(Arg::reserve = m_style.size());

			for (const auto& [key, value] : m_style)
			{
				if (std::holds_alternative<float>(value))
				{
					tmp.emplace_back(key, U"{}"_fmt(std::get<float>(value)));
				}
				else
				{
					tmp.emplace_back(key, std::get<String>(value));
				}
			}

			return Util::DumpInlineCSS(tmp);
		}
		else if (const auto it = m_additonalProperties.find(key);
			it != m_additonalProperties.end())
		{
			return it->second;
		}

		return none;
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
			m_style.clear();
			for (const auto& [key, value] : Util::ParseInlineCSS(String{ value }))
			{
				m_style.emplace(key, value);
			}
		}
		else
		{
			m_additonalProperties[key] = value;
		}
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
			m_style.clear();
		}
		else
		{
			m_additonalProperties.erase(key);
		}
	}

	void FlexBoxImpl::clearProperties()
	{
		m_id.reset();
		m_classes.clear();
		m_style.clear();
		m_additonalProperties.clear();
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

	void FlexBoxImpl::setChildren(Array<std::shared_ptr<FlexBoxImpl>>& children)
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

	FlexBoxImpl::~FlexBoxImpl()
	{
		// 先に子要素のデストラクタを呼び出させる
		removeChildren();

		// YGNodeを解放
		YGNodeFree(m_node);
		m_node = nullptr;
	}
}
