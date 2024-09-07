#include "Thickness.hpp"

void FlexLayout::Thickness::drawPadding(const RectF& rect, const ColorF& color) const
{
	if (not *this)
	{
		return;
	}

	if (top > 0)
	{
		RectF{
			rect.leftX(),
			rect.topY(),
			rect.w,
			top
		}.draw(color);
	}
	if (left > 0)
	{
		RectF{
			rect.leftX(),
			rect.topY() + top,
			left,
			rect.h - top - bottom
		}.draw(color);
	}
	if (right > 0)
	{
		RectF{
			rect.rightX() - right,
			rect.topY() + top,
			right,
			rect.h - top - bottom
		}.draw(color);
	}
	if (bottom > 0)
	{
		RectF{
			rect.leftX(),
			rect.bottomY() - bottom,
			rect.w,
			bottom
		}.draw(color);
	}
}
