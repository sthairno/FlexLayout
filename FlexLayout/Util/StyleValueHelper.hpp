#pragma once
#include "../Style/StyleValue.hpp"

#define FLEXLAYOUT_STYLE_VALUE_LITERALS(Suffix, Unit) \
	[[nodiscard]] \
	inline Style::StyleValue operator""_##Suffix(long double value) noexcept \
	{ \
		return Style::StyleValue::Length(static_cast<float>(value), Unit); \
	} \
	[[nodiscard]] \
	inline Style::StyleValue operator""_##Suffix(unsigned long long value) noexcept \
	{ \
		return Style::StyleValue::Length(static_cast<float>(value), Unit); \
	}

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
		FLEXLAYOUT_STYLE_VALUE_LITERALS(px, LengthUnit::Pixel)

		FLEXLAYOUT_STYLE_VALUE_LITERALS(ch, LengthUnit::Ch)

		FLEXLAYOUT_STYLE_VALUE_LITERALS(em, LengthUnit::Em)

		FLEXLAYOUT_STYLE_VALUE_LITERALS(ex, LengthUnit::Ex)

		FLEXLAYOUT_STYLE_VALUE_LITERALS(ic, LengthUnit::Ic)

		FLEXLAYOUT_STYLE_VALUE_LITERALS(lh, LengthUnit::Lh)
	}
}

#undef FLEXLAYOUT_STYLE_VALUE_LITERALS
