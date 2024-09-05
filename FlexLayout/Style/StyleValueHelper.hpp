#pragma once
#include "../Common.hpp"
#include "StyleValue.hpp"

namespace FlexLayout
{
	[[nodiscard]]
	inline Style::StyleValue Auto() noexcept
	{
		return Style::StyleValue::Auto();
	}

	[[nodiscard]]
	inline Style::StyleValue Pixel(float value) noexcept
	{
		return Style::StyleValue::Length(value, LengthUnit::Pixel);
	}

	[[nodiscard]]
	inline Style::StyleValue Ch(float value) noexcept
	{
		return Style::StyleValue::Length(value, LengthUnit::Ch);
	}

	[[nodiscard]]
	inline Style::StyleValue Em(float value) noexcept
	{
		return Style::StyleValue::Length(value, LengthUnit::Em);
	}

	[[nodiscard]]
	inline Style::StyleValue Ch(float value) noexcept
	{
		return Style::StyleValue::Length(value, LengthUnit::Ch);
	}

	[[nodiscard]]
	inline Style::StyleValue Ex(float value) noexcept
	{
		return Style::StyleValue::Length(value, LengthUnit::Ex);
	}

	[[nodiscard]]
	inline Style::StyleValue Ic(float value) noexcept
	{
		return Style::StyleValue::Length(value, LengthUnit::Ic);
	}

	[[nodiscard]]
	inline Style::StyleValue Lh(float value) noexcept
	{
		return Style::StyleValue::Length(value, LengthUnit::Lh);
	}

	namespace Literals
	{
		Style::StyleValue operator""_px(long double value)
		{
			return Style::StyleValue::Length(value, LengthUnit::Pixel);
		}

		Style::StyleValue operator""_ch(long double value)
		{
			return Style::StyleValue::Length(value, LengthUnit::Ch);
		}

		Style::StyleValue operator""_em(long double value)
		{
			return Style::StyleValue::Length(value, LengthUnit::Em);
		}

		Style::StyleValue operator""_ch(long double value)
		{
			return Style::StyleValue::Length(value, LengthUnit::Ch);
		}

		Style::StyleValue operator""_ex(long double value)
		{
			return Style::StyleValue::Length(value, LengthUnit::Ex);
		}

		Style::StyleValue operator""_ic(long double value)
		{
			return Style::StyleValue::Length(value, LengthUnit::Ic);
		}

		Style::StyleValue operator""_lh(long double value)
		{
			return Style::StyleValue::Length(value, LengthUnit::Lh);
		}
	}
}
