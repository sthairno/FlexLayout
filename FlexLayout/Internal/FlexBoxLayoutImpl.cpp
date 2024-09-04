#include "FlexBoxImpl.hpp"

namespace FlexLayout::Internal
{
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

	RectF FlexBoxImpl::localMarginAreaRect() const
	{
		return RectF{
			YGNodeLayoutGetLeft(m_node),
			YGNodeLayoutGetTop(m_node),
			YGNodeLayoutGetWidth(m_node),
			YGNodeLayoutGetHeight(m_node)
		};
	}

	RectF FlexBoxImpl::localBorderAreaRect() const
	{
		return localMarginAreaRect().stretched(
			-YGNodeLayoutGetMargin(m_node, YGEdgeLeft),
			-YGNodeLayoutGetMargin(m_node, YGEdgeTop),
			-YGNodeLayoutGetMargin(m_node, YGEdgeRight),
			-YGNodeLayoutGetMargin(m_node, YGEdgeBottom)
		);
	}

	RectF FlexBoxImpl::localPaddingAreaRect() const
	{
		return localBorderAreaRect().stretched(
			-YGNodeLayoutGetBorder(m_node, YGEdgeLeft),
			-YGNodeLayoutGetBorder(m_node, YGEdgeTop),
			-YGNodeLayoutGetBorder(m_node, YGEdgeRight),
			-YGNodeLayoutGetBorder(m_node, YGEdgeBottom)
		);
	}

	RectF FlexBoxImpl::localContentAreaRect() const
	{
		return localPaddingAreaRect().stretched(
			-YGNodeLayoutGetPadding(m_node, YGEdgeLeft),
			-YGNodeLayoutGetPadding(m_node, YGEdgeTop),
			-YGNodeLayoutGetPadding(m_node, YGEdgeRight),
			-YGNodeLayoutGetPadding(m_node, YGEdgeBottom)
		);
	}

	Optional<RectF> FlexBoxImpl::marginAreaRect() const
	{
		return m_layoutOffset
			? localMarginAreaRect().movedBy(*m_layoutOffset)
			: Optional<RectF>{ };
	}

	Optional<RectF> FlexBoxImpl::borderAreaRect() const
	{
		return m_layoutOffset
			? localBorderAreaRect().movedBy(*m_layoutOffset)
			: Optional<RectF>{ };
	}

	Optional<RectF> FlexBoxImpl::paddingAreaRect() const
	{
		return m_layoutOffset
			? localPaddingAreaRect().movedBy(*m_layoutOffset)
			: Optional<RectF>{ };
	}

	Optional<RectF> FlexBoxImpl::contentAreaRect() const
	{
		return m_layoutOffset
			? localContentAreaRect().movedBy(*m_layoutOffset)
			: Optional<RectF>{ };
	}

}
