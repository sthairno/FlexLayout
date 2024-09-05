#include "FlexBoxImpl.hpp"

// FlexBoxImplのレイアウトに関する実装

namespace FlexLayout::Internal
{
	void FlexBoxImpl::CalculateLayout(FlexBoxImpl& node, Optional<float> width, Optional<float> height)
	{
		YGNodeCalculateLayout(
			node.m_node,
			width.value_or(YGUndefined),
			height.value_or(YGUndefined),
			YGDirectionLTR);
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

	Thickness FlexBoxImpl::margin() const
	{
		return Thickness{
			YGNodeLayoutGetMargin(m_node, YGEdgeTop),
			YGNodeLayoutGetMargin(m_node, YGEdgeRight),
			YGNodeLayoutGetMargin(m_node, YGEdgeBottom),
			YGNodeLayoutGetMargin(m_node, YGEdgeLeft)
		};
	}

	Thickness FlexBoxImpl::border() const
	{
		return Thickness{
			YGNodeLayoutGetBorder(m_node, YGEdgeTop),
			YGNodeLayoutGetBorder(m_node, YGEdgeRight),
			YGNodeLayoutGetBorder(m_node, YGEdgeBottom),
			YGNodeLayoutGetBorder(m_node, YGEdgeLeft)
		};
	}

	Thickness FlexBoxImpl::padding() const
	{
		return Thickness{
			YGNodeLayoutGetPadding(m_node, YGEdgeTop),
			YGNodeLayoutGetPadding(m_node, YGEdgeRight),
			YGNodeLayoutGetPadding(m_node, YGEdgeBottom),
			YGNodeLayoutGetPadding(m_node, YGEdgeLeft)
		};
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
}
