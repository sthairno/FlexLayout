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
}
