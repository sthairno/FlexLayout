#include "Box.hpp"
#include "Label.hpp"
#include "Internal/FlexBoxImpl.hpp"

namespace FlexLayout
{
	Optional<Vec2> Box::offset() const
	{
		return m_impl->layoutOffset();
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

	RectF Box::localRect() const
	{
		return m_impl->localBorderAreaRect();
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
		return m_impl->getStyle(styleName);
	}

	bool Box::setStyle(const StringView styleName, const Array<StyleValue>& list)
	{
		return m_impl->setStyle(styleName, std::span{ list.begin(), list.end() });
	}

	bool Box::setStyle(const StringView styleName, StyleValue value)
	{
		return m_impl->setStyle(styleName, std::array{ value });
	}

	bool Box::setStyle(const StringView styleName, StyleValue v1, StyleValue v2)
	{
		return m_impl->setStyle(styleName, std::array{ v1, v2 });
	}

	bool Box::setStyle(const StringView styleName, StyleValue v1, StyleValue v2, StyleValue v3)
	{
		return m_impl->setStyle(styleName, std::array{ v1, v2, v3 });
	}

	bool Box::setStyle(const StringView styleName, StyleValue v1, StyleValue v2, StyleValue v3, StyleValue v4)
	{
		return m_impl->setStyle(styleName, std::array{ v1, v2, v3, v4 });
	}

	bool Box::setStyle(
		const StringView styleName,
		Style::ValueInputVariant value
	)
	{
		return m_impl->setStyle(styleName, std::array{ value });
	}

	bool Box::setStyle(
		const StringView styleName,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2
	)
	{
		return m_impl->setStyle(styleName, std::array{ v1, v2 });
	}

	bool Box::setStyle(
		const StringView styleName,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2,
		Style::ValueInputVariant v3
	)
	{
		return m_impl->setStyle(styleName, std::array{ v1, v2, v3 });
	}

	bool Box::setStyle(
		const StringView styleName,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2,
		Style::ValueInputVariant v3,
		Style::ValueInputVariant v4
	)
	{
		return m_impl->setStyle(styleName, std::array{ v1, v2, v3, v4 });
	}

	Array<Box> Box::children() const
	{
		return m_impl->children().map([](const auto& child) { return Box{ child }; });
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
		if (m_impl->isLabel())
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
