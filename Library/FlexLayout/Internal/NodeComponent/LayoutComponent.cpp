#include "LayoutComponent.hpp"
#include <yoga/Yoga.h>
#include "../FlexBoxNode.hpp"

namespace FlexLayout::Internal::Component
{
	LayoutComponent::LayoutComponent(FlexBoxNode& node)
		: m_node(node) { }

	void LayoutComponent::copy(const LayoutComponent& source)
	{
		m_propergateOffsetToChildren = source.m_propergateOffsetToChildren;
	}

	void LayoutComponent::setLayoutOffsetRecursive(Optional<Vec2> offset, bool force)
	{
		// 更新部分だけ処理する
		// https://www.yogalayout.dev/docs/advanced/incremental-layout
		if (not force && offset == m_layoutOffset)
		{
			if (not YGNodeGetHasNewLayout(m_node.yogaNode()))
			{
				return;
			}
		}
		YGNodeSetHasNewLayout(m_node.yogaNode(), false);

		Optional<Vec2> childOffset;
		if (offset && YGNodeStyleGetDisplay(m_node.yogaNode()) != YGDisplayNone)
		{
			m_layoutOffset = *offset;
			childOffset = m_propergateOffsetToChildren
				? *offset + Vec2{ YGNodeLayoutGetLeft(m_node.yogaNode()), YGNodeLayoutGetTop(m_node.yogaNode()) }
			: Vec2::Zero();
		}
		else
		{
			m_layoutOffset.reset();
		}

		for (const auto& child : m_node.children())
		{
			child->getComponent<LayoutComponent>().setLayoutOffsetRecursive(childOffset);
		}
	}

	void LayoutComponent::clearLayoutOffsetRecursive()
	{
		m_layoutOffset.reset();
		for (const auto& child : m_node.children())
		{
			child->getComponent<LayoutComponent>().clearLayoutOffsetRecursive();
		}
	}

	Thickness LayoutComponent::margin() const
	{
		return Thickness{
			YGNodeLayoutGetMargin(m_node.yogaNode(), YGEdgeTop),
			YGNodeLayoutGetMargin(m_node.yogaNode(), YGEdgeRight),
			YGNodeLayoutGetMargin(m_node.yogaNode(), YGEdgeBottom),
			YGNodeLayoutGetMargin(m_node.yogaNode(), YGEdgeLeft)
		};
	}

	Thickness LayoutComponent::border() const
	{
		return Thickness{
			YGNodeLayoutGetBorder(m_node.yogaNode(), YGEdgeTop),
			YGNodeLayoutGetBorder(m_node.yogaNode(), YGEdgeRight),
			YGNodeLayoutGetBorder(m_node.yogaNode(), YGEdgeBottom),
			YGNodeLayoutGetBorder(m_node.yogaNode(), YGEdgeLeft)
		};
	}

	Thickness LayoutComponent::padding() const
	{
		return Thickness{
			YGNodeLayoutGetPadding(m_node.yogaNode(), YGEdgeTop),
			YGNodeLayoutGetPadding(m_node.yogaNode(), YGEdgeRight),
			YGNodeLayoutGetPadding(m_node.yogaNode(), YGEdgeBottom),
			YGNodeLayoutGetPadding(m_node.yogaNode(), YGEdgeLeft)
		};
	}

	RectF LayoutComponent::localBorderAreaRect() const
	{
		return RectF{
			YGNodeLayoutGetLeft(m_node.yogaNode()),
			YGNodeLayoutGetTop(m_node.yogaNode()),
			YGNodeLayoutGetWidth(m_node.yogaNode()),
			YGNodeLayoutGetHeight(m_node.yogaNode())
		};
	}
}
