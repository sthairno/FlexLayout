#pragma once
#include <Siv3D/Vector2D.hpp>
#include "../../Thickness.hpp"
#include <Siv3D/RectF.hpp>

using namespace s3d;

namespace FlexLayout::Internal
{
	class FlexBoxNode;
}

namespace FlexLayout::Internal::Component
{
	class LayoutComponent
	{
	public:

		LayoutComponent(FlexBoxNode& node);

	public:

		void copy(const LayoutComponent& source);

		Optional<Vec2> layoutOffset() const { return m_layoutOffset; }

		void setLayoutOffsetRecursive(Optional<Vec2> offset, bool force = false);

		void clearLayoutOffsetRecursive();

		bool propergateOffset() const { return m_propergateOffsetToChildren; }

		inline void setPropergateOffset(bool propergate) { m_propergateOffsetToChildren = propergate; }

		Thickness margin() const;

		Thickness border() const;

		Thickness padding() const;

		inline RectF localMarginAreaRect() const
		{
			return margin().expandRect(localBorderAreaRect());
		}

		RectF localBorderAreaRect() const;

		inline RectF localPaddingAreaRect() const
		{
			return border().shrinkRect(localBorderAreaRect());
		}

		inline RectF localContentAreaRect() const
		{
			return padding().shrinkRect(localPaddingAreaRect());
		}

		inline Optional<RectF> marginAreaRect() const
		{
			return m_layoutOffset
				? localMarginAreaRect().movedBy(*m_layoutOffset)
				: Optional<RectF>{};
		}

		inline Optional<RectF> borderAreaRect() const
		{
			return m_layoutOffset
				? localBorderAreaRect().movedBy(*m_layoutOffset)
				: Optional<RectF>{};
		}

		inline Optional<RectF> paddingAreaRect() const
		{
			return m_layoutOffset
				? localPaddingAreaRect().movedBy(*m_layoutOffset)
				: Optional<RectF>{};
		}

		inline Optional<RectF> contentAreaRect() const
		{
			return m_layoutOffset
				? localContentAreaRect().movedBy(*m_layoutOffset)
				: Optional<RectF>{};
		}

	private:

		FlexBoxNode& m_node;

		/// @brief ローカル座標からグローバル座標へ変換
		Optional<Vec2> m_layoutOffset;

		/// @brief `setLayoutOffsetRecursive`呼び出し時に、子要素にオフセットを伝播させる
		bool m_propergateOffsetToChildren = true;
	};
}
