#include "XmlAttributeComponent.hpp"
#include <Siv3D/HeterogeneousLookupHelper.hpp>
#include "../FlexBoxNode.hpp"

namespace FlexLayout::Internal::Component
{
	XmlAttributeComponent::XmlAttributeComponent(FlexBoxNode& node)
		: m_node{ node } { }

	void XmlAttributeComponent::copy(const XmlAttributeComponent& source)
	{
		m_tagName = source.m_tagName;
		m_id = source.m_id;
		m_classes = source.m_classes;
	}

	void XmlAttributeComponent::setId(const Optional<String>& id)
	{
		assert(not id || not id->isEmpty());

		m_id = id;
	}

	void XmlAttributeComponent::setClasses(const Array<String>& classes)
	{
		for (const auto& className : classes)
		{
			assert(not className.isEmpty());
		}

		m_classes = classes;
	}

	String XmlAttributeComponent::getClassText() const
	{
		return m_classes.join(U" ", U"", U"");
	}

	void XmlAttributeComponent::setClassText(const StringView classText)
	{
		m_classes.clear();
		for (auto& className : String{ classText }.split(U' '))
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

	bool XmlAttributeComponent::addClass(const StringView className)
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

	bool XmlAttributeComponent::removeClass(const StringView className)
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

	bool XmlAttributeComponent::lookupNodeByInstance(const std::shared_ptr<FlexBoxNode>& node)
	{
		if (m_node.shared_from_this() == node)
		{
			return true;
		}

		for (const auto& child : m_node.children())
		{
			if (child->getComponent<XmlAttributeComponent>().lookupNodeByInstance(node))
			{
				return true;
			}
		}

		return false;
	}

	void XmlAttributeComponent::lookupNodesByClassName(Array<std::shared_ptr<FlexBoxNode>>& list, const String& className, size_t limit)
	{
		if (list.size() >= limit)
		{
			return;
		}

		if (m_classes.contains(className))
		{
			list.push_back(m_node.shared_from_this());
		}

		for (const auto& child : m_node.children())
		{
			child->getComponent<XmlAttributeComponent>()
				.lookupNodesByClassName(list, className, limit);
		}
	}

	std::shared_ptr<FlexBoxNode> XmlAttributeComponent::lookupNodeById(const StringView id)
	{
		if (m_id == id)
		{
			return m_node.shared_from_this();
		}

		for (const auto& child : m_node.children())
		{
			if (auto node = child->getComponent<XmlAttributeComponent>().lookupNodeById(id))
			{
				return node;
			}
		}

		return { };
	}
}
