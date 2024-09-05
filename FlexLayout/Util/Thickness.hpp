﻿#pragma once
#include "../Common.hpp"

namespace FlexLayout
{
	struct Thickness
	{
		using value_type = float;

		[[nodiscard]]
		explicit constexpr Thickness() noexcept = default;

		[[nodiscard]]
		explicit constexpr Thickness(value_type _all) noexcept
			: top(_all)
			, right(_all)
			, bottom(_all)
			, left(_all) {}

		[[nodiscard]]
		explicit constexpr Thickness(value_type _vertical, value_type _horizontal) noexcept
			: top(_vertical)
			, right(_horizontal)
			, bottom(_vertical)
			, left(_horizontal) {}

		[[nodiscard]]
		explicit constexpr Thickness(value_type _top, value_type _horizontal, value_type _bottom) noexcept
			: top(_top)
			, right(_horizontal)
			, bottom(_bottom)
			, left(_horizontal) {}

		[[nodiscard]]
		explicit constexpr Thickness(value_type _top, value_type _right, value_type _bottom, value_type _left) noexcept
			: top(_top)
			, right(_right)
			, bottom(_bottom)
			, left(_left) {}

		value_type top = 0.0;
		value_type right = 0.0;
		value_type bottom = 0.0;
		value_type left = 0.0;

		[[nodiscard]]
		friend constexpr bool operator ==(const Rect& lhs, const Rect& rhs) noexcept
		{
			return (lhs.pos == rhs.pos)
				&& (lhs.size == rhs.size);
		}

		[[nodiscard]]
		friend constexpr bool operator !=(const Rect& lhs, const Rect& rhs) noexcept
		{
			return (lhs.pos != rhs.pos)
				|| (lhs.size != rhs.size);
		}

		[[nodiscard]]
		constexpr Thickness operator +(value_type value) const noexcept
		{
			return Thickness{ top + value, right + value, bottom + value, left + value };
		}

		[[nodiscard]]
		constexpr Thickness operator -(value_type value) const noexcept
		{
			return Thickness{ top - value, right - value, bottom - value, left - value };
		}

		[[nodiscard]]
		constexpr Thickness operator *(value_type value) const noexcept
		{
			return Thickness{ top * value, right * value, bottom * value, left * value };
		}

		[[nodiscard]]
		constexpr Thickness operator /(value_type value) const noexcept
		{
			return Thickness{ top / value, right / value, bottom / value, left / value };
		}

		constexpr Thickness& operator +=(value_type value) noexcept
		{
			top += value;
			right += value;
			bottom += value;
			left += value;
			return *this;
		}

		constexpr Thickness& operator -=(value_type value) noexcept
		{
			top -= value;
			right -= value;
			bottom -= value;
			left -= value;
			return *this;
		}

		constexpr Thickness& operator *=(value_type value) noexcept
		{
			top *= value;
			right *= value;
			bottom *= value;
			left *= value;
			return *this;
		}

		constexpr Thickness& operator /=(value_type value) noexcept
		{
			top /= value;
			right /= value;
			bottom /= value;
			left /= value;
			return *this;
		}

		[[nodiscard]]
		constexpr explicit operator bool() const noexcept
		{
			return (top != 0.0)
				|| (right != 0.0)
				|| (bottom != 0.0)
				|| (left != 0.0);
		}

		[[nodiscard]]
		constexpr Thickness operator -() const noexcept
		{
			return Thickness{ -top, -right, -bottom, -left };
		}

		[[nodiscard]]
		constexpr Thickness operator +() const noexcept
		{
			return *this;
		}

		[[nodiscard]]
		constexpr Thickness operator +(const Thickness& other) const noexcept
		{
			return Thickness{ top + other.top, right + other.right, bottom + other.bottom, left + other.left };
		}

		[[nodiscard]]
		constexpr Thickness operator -(const Thickness& other) const noexcept
		{
			return Thickness{ top - other.top, right - other.right, bottom - other.bottom, left - other.left };
		}

		[[nodiscard]]
		constexpr SizeF shrinkSize(const SizeF& size) const noexcept
		{
			return SizeF{
				size.x - left - right,
				size.y - top - bottom
			};
		}

		[[nodiscard]]
		constexpr RectF shrinkRect(const RectF& rect) const noexcept
		{
			return RectF{
				rect.x + left,
				rect.y + top,
				rect.w - left - right,
				rect.h - top - bottom
			};
		}

		[[nodiscard]]
		constexpr SizeF expandSize(const SizeF& size) const noexcept
		{
			return SizeF{
				size.x + left + right,
				size.y + top + bottom
			};
		}

		[[nodiscard]]
		constexpr RectF expandRect(const RectF& rect) const noexcept
		{
			return RectF{
				rect.x - left,
				rect.y - top,
				rect.w + left + right,
				rect.h + top + bottom
			};
		}

		[[nodiscard]]
		constexpr Vec4 asVec4() const noexcept
		{
			return Vec4{ top, right, bottom, left };
		}

		void drawPadding(const RectF& rect, const ColorF& color) const;
	};
}
