﻿#include "UIState.hpp"
#include "Internal/FlexBoxNode.hpp"
#include "Internal/Style/StyleProperty.hpp"
#include "Internal/NodeComponent/StyleComponent.hpp"

using namespace s3d;

namespace FlexLayout
{
	Array<Style::StyleValue> UIStateQuery::getStyle(const StringView styleName) const
	{
		return m_node
			.getComponent<Internal::Component::StyleComponent>()
			.getStyle(Internal::StylePropertyGroup::Preset, styleName);
	}

	bool UIStateQuery::setStyle(const StringView styleName, const Array<Style::StyleValue>& list)
	{
		return m_node
			.getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Preset, styleName, std::span{ list.begin(), list.end() });
	}

	bool UIStateQuery::setStyle(const StringView styleName, Style::StyleValue value)
	{
		return m_node
			.getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Preset, styleName, std::array{ value });
	}

	bool UIStateQuery::setStyle(const StringView styleName, Style::StyleValue v1, Style::StyleValue v2)
	{
		return m_node
			.getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Preset, styleName, std::array{ v1, v2 });
	}

	bool UIStateQuery::setStyle(const StringView styleName, Style::StyleValue v1, Style::StyleValue v2, Style::StyleValue v3)
	{
		return m_node
			.getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Preset, styleName, std::array{ v1, v2, v3 });
	}

	bool UIStateQuery::setStyle(const StringView styleName, Style::StyleValue v1, Style::StyleValue v2, Style::StyleValue v3, Style::StyleValue v4)
	{
		return m_node
			.getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Preset, styleName, std::array{ v1, v2, v3, v4 });
	}

	bool UIStateQuery::setStyle(
		const StringView styleName,
		Style::ValueInputVariant value
	)
	{
		return m_node
			.getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Preset, styleName, std::array{ value });
	}

	bool UIStateQuery::setStyle(
		const StringView styleName,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2
	)
	{
		return m_node
			.getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Preset, styleName, std::array{ v1, v2 });
	}

	bool UIStateQuery::setStyle(
		const StringView styleName,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2,
		Style::ValueInputVariant v3
	)
	{
		return m_node
			.getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Preset, styleName, std::array{ v1, v2, v3 });
	}

	bool UIStateQuery::setStyle(
		const StringView styleName,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2,
		Style::ValueInputVariant v3,
		Style::ValueInputVariant v4
	)
	{
		return m_node
			.getComponent<Internal::Component::StyleComponent>()
			.setStyle(Internal::StylePropertyGroup::Preset, styleName, std::array{ v1, v2, v3, v4 });
	}

	bool UIStateQuery::unsetStyle(const s3d::StringView styleName)
	{
		return m_node
			.getComponent<Internal::Component::StyleComponent>()
			.removeStyle(Internal::StylePropertyGroup::Preset, styleName);
	}

	s3d::Font UIStateQuery::font() const
	{
		return m_node
			.getComponent<Internal::Component::StyleComponent>()
			.font();
	}

	void UIStateQuery::setFont(s3d::Font font)
	{
		m_node
			.getComponent<Internal::Component::StyleComponent>()
			.setFont(font);
	}

	void UIStateQuery::unsetFont()
	{
		m_node
			.getComponent<Internal::Component::StyleComponent>()
			.setFont(Font{});
	}

	Optional<String> UIStateQuery::getAttribute(s3d::StringView name) const
	{
		return m_node.getProperty(name);
	}

	void UIStateQuery::setAttribute(s3d::StringView name, s3d::StringView value)
	{
		m_node.setProperty(name, value);
	}

	bool UIStateQuery::hasAttribute(s3d::StringView name) const
	{
		return m_node.getProperty(name).has_value();
	}

	bool UIStateQuery::removeAttribute(s3d::StringView name)
	{
		return m_node.removeProperty(name);
	}
}
