#include "FlexBox.hpp"
#include "detail/FlexBoxImpl.hpp"

namespace FlexLayout
{
	Optional<Vec2> FlexBox::offset() const
	{
		return m_impl->layoutOffset();
	}

	RectF FlexBox::localRect() const
	{
		auto node = m_impl->yogaNode();
		return RectF{
			YGNodeLayoutGetLeft(node),
			YGNodeLayoutGetTop(node),
			YGNodeLayoutGetWidth(node),
			YGNodeLayoutGetHeight(node)
		};
	}

	Optional<RectF> FlexBox::marginAreaRect() const
	{
		if (auto offset = m_impl->layoutOffset())
		{
			auto node = m_impl->yogaNode();
			return RectF{
				YGNodeLayoutGetLeft(node) + offset->x,
				YGNodeLayoutGetTop(node) + offset->y,
				YGNodeLayoutGetWidth(node),
				YGNodeLayoutGetHeight(node)
			};
		}

		return none;
	}

	Optional<RectF> FlexBox::borderAreaRect() const
	{
		if (auto rect = marginAreaRect())
		{
			auto node = m_impl->yogaNode();
			return rect->stretched(
				-YGNodeLayoutGetMargin(node, YGEdgeLeft),
				-YGNodeLayoutGetMargin(node, YGEdgeTop),
				-YGNodeLayoutGetMargin(node, YGEdgeRight),
				-YGNodeLayoutGetMargin(node, YGEdgeBottom)
			);
		}

		return none;
	}

	Optional<RectF> FlexBox::paddingAreaRect() const
	{
		if (auto rect = marginAreaRect())
		{
			auto node = m_impl->yogaNode();
			return rect->stretched(
				-YGNodeLayoutGetBorder(node, YGEdgeLeft),
				-YGNodeLayoutGetBorder(node, YGEdgeTop),
				-YGNodeLayoutGetBorder(node, YGEdgeRight),
				-YGNodeLayoutGetBorder(node, YGEdgeBottom)
			);
		}

		return none;
	}

	Optional<RectF> FlexBox::contentAreaRect() const
	{
		if (auto rect = paddingAreaRect())
		{
			auto node = m_impl->yogaNode();
			return rect->stretched(
				-YGNodeLayoutGetPadding(node, YGEdgeLeft),
				-YGNodeLayoutGetPadding(node, YGEdgeTop),
				-YGNodeLayoutGetPadding(node, YGEdgeRight),
				-YGNodeLayoutGetPadding(node, YGEdgeBottom)
			);
		}

		return none;
	}

	void FlexBox::draw(const ColorF& color) const
	{
		auto renderer = m_impl->labelRenderer();
		auto rect = contentAreaRect();
		if (renderer && rect)
		{
			renderer->draw(*rect, color);
		}
	}

	Array<FlexBox> FlexBox::children() const
	{
		return m_impl->children().map([](const auto& child) { return FlexBox{ child }; });
	}

	Array<FlexBox> FlexBox::getElementsByClassName(StringView className) const
	{
		Array<std::shared_ptr<detail::FlexBoxImpl>> list;
		m_impl->lookupNodesByClassName(list, String{ className }, Largest<size_t>);
		return list.map([](const auto& node) { return FlexBox{ node }; });
	}

	Optional<FlexBox> FlexBox::getElementById(StringView id) const
	{
		auto impl = m_impl->lookupNodeById(id);

		if (impl)
		{
			return FlexBox{ impl };
		}

		return none;
	}
}
