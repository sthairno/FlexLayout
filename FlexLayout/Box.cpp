#include "Box.hpp"
#include "Label.hpp"
#include "Internal/FlexBoxImpl.hpp"

namespace FlexLayout
{
	Optional<Vec2> Box::offset() const
	{
		return m_impl->layoutOffset();
	}

	RectF Box::localRect() const
	{
		return m_impl->localMarginAreaRect();
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

	bool Box::setStyle(const StringView key, const Array<StyleValue>& list)
	{
		return m_impl->setStyle(key, std::span{ list.begin(), list.end() });
	}

	bool Box::setStyle(const StringView key, StyleValue value)
	{
		return m_impl->setStyle(key, std::array{ value });
	}

	bool Box::setStyle(const StringView key, StyleValue v1, StyleValue v2)
	{
		return m_impl->setStyle(key, std::array{ v1, v2 });
	}

	bool Box::setStyle(const StringView key, StyleValue v1, StyleValue v2, StyleValue v3)
	{
		return m_impl->setStyle(key, std::array{ v1, v2, v3 });
	}

	bool Box::setStyle(const StringView key, StyleValue v1, StyleValue v2, StyleValue v3, StyleValue v4)
	{
		return m_impl->setStyle(key, std::array{ v1, v2, v3, v4 });
	}

	bool Box::setStyle(
		const StringView key,
		Style::ValueInputVariant value
	)
	{
		return m_impl->setStyle(key, std::array{ value });
	}

	bool Box::setStyle(
		const StringView key,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2
	)
	{
		return m_impl->setStyle(key, std::array{ v1, v2 });
	}

	bool Box::setStyle(
		const StringView key,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2,
		Style::ValueInputVariant v3
	)
	{
		return m_impl->setStyle(key, std::array{ v1, v2, v3 });
	}

	bool Box::setStyle(
		const StringView key,
		Style::ValueInputVariant v1,
		Style::ValueInputVariant v2,
		Style::ValueInputVariant v3,
		Style::ValueInputVariant v4
	)
	{
		return m_impl->setStyle(key, std::array{ v1, v2, v3, v4 });
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

	Box::~Box() { }
}
