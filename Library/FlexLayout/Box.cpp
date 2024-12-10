#include "Box.hpp"
#include "Label.hpp"
#include "Internal/FlexBoxNode.hpp"

#include "Internal/NodeComponent/LayoutComponent.hpp"
#include "Internal/NodeComponent/StyleComponent.hpp"
#include "Internal/NodeComponent/XmlAttributeComponent.hpp"

namespace FlexLayout
{
	Box::Box(const std::shared_ptr<Internal::FlexBoxNode>& node)
		: m_node(node) { }

	Optional<Vec2> Box::offset() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.layoutOffset();
	}

	bool Box::propergateOffset() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.propergateOffset();
	}

	void Box::setPropergateOffset(bool propergate)
	{
		m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.setPropergateOffset(propergate);
	}

	Thickness Box::margin() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.margin();
	}

	Thickness Box::border() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.border();
	}

	Thickness Box::padding() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.padding();
	}
	
	RectF Box::localMarginAreaRect() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.localMarginAreaRect();
	}

	RectF Box::localBorderAreaRect() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.localBorderAreaRect();
	}

	RectF Box::localPaddingAreaRect() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.localPaddingAreaRect();
	}

	RectF Box::localContentAreaRect() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.localContentAreaRect();
	}

	Optional<RectF> Box::marginAreaRect() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.marginAreaRect();
	}

	Optional<RectF> Box::borderAreaRect() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.borderAreaRect();
	}

	Optional<RectF> Box::paddingAreaRect() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.paddingAreaRect();
	}

	Optional<RectF> Box::contentAreaRect() const
	{
		return m_node
			->getComponent<Internal::Component::LayoutComponent>()
			.contentAreaRect();
	}

	Array<StyleValue> Box::getStyle(const StringView styleName) const
	{
		return m_node
			->getComponent<Internal::Component::StyleComponent>()
			.getStyle(Internal::StylePropertyGroup::Inline, styleName);
	}

	bool Box::setStyle(const StringView styleName, const Array<StyleValue>& list)
	{
		return m_node
			->getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Inline, styleName, std::span{ list.begin(), list.end() });
	}

	bool Box::setStyle(const StringView styleName, StyleValue value)
	{
		return m_node
			->getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ value });
	}

	bool Box::setStyle(const StringView styleName, StyleValue v1, StyleValue v2)
	{
		return m_node
			->getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ v1, v2 });
	}

	bool Box::setStyle(const StringView styleName, StyleValue v1, StyleValue v2, StyleValue v3)
	{
		return m_node
			->getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ v1, v2, v3 });
	}

	bool Box::setStyle(const StringView styleName, StyleValue v1, StyleValue v2, StyleValue v3, StyleValue v4)
	{
		return m_node
			->getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ v1, v2, v3, v4 });
	}

	bool Box::setStyle(
		const StringView styleName,
		Style::ValueInputVariant value
	)
	{
		return m_node
			->getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ value });
	}

	bool Box::setStyle(
		const StringView styleName,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2
	)
	{
		return m_node
			->getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ v1, v2 });
	}

	bool Box::setStyle(
		const StringView styleName,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2,
		Style::ValueInputVariant v3
	)
	{
		return m_node
			->getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ v1, v2, v3 });
	}

	bool Box::setStyle(
		const StringView styleName,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2,
		Style::ValueInputVariant v3,
		Style::ValueInputVariant v4
	)
	{
		return m_node
			->getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ v1, v2, v3, v4 });
	}

	bool Box::unsetStyle(const s3d::StringView styleName)
	{
		return m_node
			->getComponent<Internal::Component::StyleComponent>()
			.removeStyle(Internal::StylePropertyGroup::Inline, styleName);
	}

	s3d::Font Box::font() const
	{
		return m_node
			->getComponent<Internal::Component::StyleComponent>()
			.font();
	}

	void Box::setFont(s3d::Font font)
	{
		m_node
			->getComponent<Internal::Component::StyleComponent>()
			.setFont(font);
	}

	void Box::unsetFont()
	{
		m_node
			->getComponent<Internal::Component::StyleComponent>()
			.setFont(Font{});
	}

	s3d::Optional<Box> Box::parent() const
	{
		return m_node->parent()
			? MakeOptional(Box{ m_node->parent()->shared_from_this() })
			: none;
	}

	Array<Box> Box::children() const
	{
		return m_node->children().map([](const auto& child) { return Box{ child }; });
	}

	void Box::removeChildren()
	{
		m_node->removeChildren();
	}

	Box Box::appendChild(Box child)
	{
		m_node->appendChild(child.m_node);
		return child;
	}

	Box Box::cloneNode(bool deep) const
	{
		if (deep)
		{
			return Box{ m_node->deepClone() };
		}
		else
		{
			return Box{ m_node->clone() };
		}
	}

	bool Box::contains(Box node) const
	{
		return m_node
			->getComponent<Internal::Component::XmlAttributeComponent>()
			.lookupNodeByInstance(node.m_node);
	}

	Box Box::getRootNode()
	{
		return Box{ m_node->getRoot().shared_from_this() };
	}

	bool Box::hasChildNodes() const
	{
		return !m_node->children().isEmpty();
	}

	Box Box::removeChild(Box child)
	{
		m_node->removeChild(child.m_node);
		return child;
	}

	void Box::replaceChildren(s3d::Array<Box> newChildren)
	{
		m_node->setChildren(newChildren.map([](const auto& child) { return child.m_node; }));
	}

	Optional<String> Box::getAttribute(s3d::StringView name) const
	{
		return m_node->getProperty(name);
	}

	void Box::setAttribute(s3d::StringView name, s3d::StringView value)
	{
		m_node->setProperty(name, value);
	}

	bool Box::hasAttribute(s3d::StringView name) const
	{
		return m_node->getProperty(name).has_value();
	}

	bool Box::removeAttribute(s3d::StringView name)
	{
		return m_node->removeProperty(name);
	}

	Array<Box> Box::getElementsByClassName(StringView className) const
	{
		Array<std::shared_ptr<Internal::FlexBoxNode>> list;
		m_node
			->getComponent<Internal::Component::XmlAttributeComponent>()
			.lookupNodesByClassName(list, String{ className }, Largest<size_t>);
		return list.map([](const auto& node) { return Box{ node }; });
	}

	Optional<Box> Box::getElementById(StringView id) const
	{
		auto impl = m_node
			->getComponent<Internal::Component::XmlAttributeComponent>()
			.lookupNodeById(id);

		if (impl)
		{
			return Box{ impl };
		}

		return none;
	}

	Optional<Label> Box::asLabel() const
	{
		if (m_node->isTextNode())
		{
			return Label{ m_node };
		}
		return none;
	}

	void Box::drawFrame(const ColorF& color) const
	{
		auto& component = m_node->getComponent<Internal::Component::LayoutComponent>();

		if (auto rect = component.borderAreaRect())
		{
			component.border().drawPadding(*rect, color);
		}
	}

	Box::~Box() { }
}
