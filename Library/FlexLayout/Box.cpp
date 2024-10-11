#include "Box.hpp"
#include "Label.hpp"
#include "Internal/FlexBoxImpl.hpp"

namespace FlexLayout
{
	Box::Box(std::shared_ptr<Internal::FlexBoxImpl> impl)
		: m_impl(std::move(impl)) { }

	Box::Box(Internal::FlexBoxImpl& impl)
		: m_impl(impl.shared_from_this()) { }

	Optional<Vec2> Box::offset() const
	{
		return m_impl->layoutOffset();
	}

	bool Box::propergateOffset() const
	{
		return m_impl->propergateOffset();
	}

	void Box::setPropergateOffset(bool propergate)
	{
		m_impl->setPropergateOffset(propergate);
	}

	Thickness Box::margin() const
	{
		return m_impl->margin();
	}

	Thickness Box::border() const
	{
		return m_impl->border();
	}

	Thickness Box::padding() const
	{
		return m_impl->padding();
	}
	
	RectF Box::localMarginAreaRect() const
	{
		return m_impl->localMarginAreaRect();
	}

	RectF Box::localBorderAreaRect() const
	{
		return m_impl->localBorderAreaRect();
	}

	RectF Box::localPaddingAreaRect() const
	{
		return m_impl->localPaddingAreaRect();
	}

	RectF Box::localContentAreaRect() const
	{
		return m_impl->localContentAreaRect();
	}

	Optional<RectF> Box::marginAreaRect() const
	{
		return m_impl->marginAreaRect();
	}

	Optional<RectF> Box::borderAreaRect() const
	{
		return m_impl->borderAreaRect();
	}

	Optional<RectF> Box::paddingAreaRect() const
	{
		return m_impl->paddingAreaRect();
	}

	Optional<RectF> Box::contentAreaRect() const
	{
		return m_impl->contentAreaRect();
	}

	Array<StyleValue> Box::getStyle(const StringView styleName) const
	{
		return m_impl->getStyle(Internal::StylePropertyGroup::Inline, styleName);
	}

	bool Box::setStyle(const StringView styleName, const Array<StyleValue>& list)
	{
		return m_impl->setStyle(Internal::StylePropertyGroup::Inline, styleName, std::span{ list.begin(), list.end() });
	}

	bool Box::setStyle(const StringView styleName, StyleValue value)
	{
		return m_impl->setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ value });
	}

	bool Box::setStyle(const StringView styleName, StyleValue v1, StyleValue v2)
	{
		return m_impl->setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ v1, v2 });
	}

	bool Box::setStyle(const StringView styleName, StyleValue v1, StyleValue v2, StyleValue v3)
	{
		return m_impl->setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ v1, v2, v3 });
	}

	bool Box::setStyle(const StringView styleName, StyleValue v1, StyleValue v2, StyleValue v3, StyleValue v4)
	{
		return m_impl->setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ v1, v2, v3, v4 });
	}

	bool Box::setStyle(
		const StringView styleName,
		Style::ValueInputVariant value
	)
	{
		return m_impl->setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ value });
	}

	bool Box::setStyle(
		const StringView styleName,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2
	)
	{
		return m_impl->setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ v1, v2 });
	}

	bool Box::setStyle(
		const StringView styleName,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2,
		Style::ValueInputVariant v3
	)
	{
		return m_impl->setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ v1, v2, v3 });
	}

	bool Box::setStyle(
		const StringView styleName,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2,
		Style::ValueInputVariant v3,
		Style::ValueInputVariant v4
	)
	{
		return m_impl->setStyle(Internal::StylePropertyGroup::Inline, styleName, std::array{ v1, v2, v3, v4 });
	}

	bool Box::unsetStyle(const s3d::StringView styleName)
	{
		return m_impl->removeStyle(Internal::StylePropertyGroup::Inline, styleName);
	}

	s3d::Font Box::font() const
	{
		return m_impl->font();
	}

	void Box::setFont(s3d::Font font)
	{
		m_impl->setFont(font);
	}

	s3d::Optional<Box> Box::parent() const
	{
		return m_impl->parent()
			? MakeOptional(Box{ *m_impl->parent() })
			: none;
	}

	Array<Box> Box::children() const
	{
		return m_impl->children().map([](const auto& child) { return Box{ child }; });
	}

	void Box::removeChildren()
	{
		m_impl->removeChildren();
	}

	Box Box::appendChild(Box child)
	{
		m_impl->appendChild(child.m_impl);
		return child;
	}

	Box Box::cloneNode() const
	{
		auto clone = m_impl->deepClone();
		return Box{ clone };
	}

	bool Box::contains(Box child) const
	{
		return m_impl->children().contains(child.m_impl);
	}

	Box Box::getRootNode()
	{
		return Box{ m_impl->getRoot() };
	}

	bool Box::hasChildNodes() const
	{
		return !m_impl->children().isEmpty();
	}

	Box Box::removeChild(Box child)
	{
		m_impl->removeChild(child.m_impl);
		return child;
	}

	void Box::replaceChildren(s3d::Array<Box> newChildren)
	{
		m_impl->setChildren(newChildren.map([](const auto& child) { return child.m_impl; }));
	}

	Optional<String> Box::getAttribute(s3d::StringView name) const
	{
		return m_impl->getProperty(name);
	}

	void Box::setAttribute(s3d::StringView name, s3d::StringView value)
	{
		m_impl->setProperty(name, value);
	}

	bool Box::hasAttribute(s3d::StringView name) const
	{
		return m_impl->getProperty(name).has_value();
	}

	bool Box::hasAttributes() const
	{
		return !m_impl->hasProperties();
	}

	void Box::removeAttribute(s3d::StringView name)
	{
		m_impl->removeProperty(name);
	}

	Array<Box> Box::getElementsByClassName(StringView className) const
	{
		Array<std::shared_ptr<Internal::FlexBoxImpl>> list;
		m_impl->lookupNodesByClassName(list, String{ className }, Largest<size_t>);
		return list.map([](const auto& node) { return Box{ node }; });
	}

	Optional<Box> Box::getElementById(StringView id) const
	{
		auto impl = m_impl->lookupNodeById(id);

		if (impl)
		{
			return Box{ impl };
		}

		return none;
	}

	Optional<Label> Box::asLabel() const
	{
		if (m_impl->type() == NodeType::Label)
		{
			return Label{ m_impl };
		}
		return none;
	}

	void Box::drawFrame(const ColorF& color) const
	{
		if (auto rect = m_impl->borderAreaRect())
		{
			m_impl->border().drawPadding(*rect, color);
		}
	}

	Box::~Box() { }
}
