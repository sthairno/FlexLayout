#pragma once
#include "../Common.hpp"
#include "StyleValue.hpp"
#include "StyleValueMatchRule.hpp"

namespace FlexLayout::Style
{
	StyleValue ParseValue(std::int32_t value, detail::StyleValueMultiMatchRule rule = StyleValue::Type::Number);

	StyleValue ParseValue(float value, detail::StyleValueMultiMatchRule rule = StyleValue::Type::Number);

	StyleValue ParseValue(const StringView str, detail::StyleValueMultiMatchRule rule = StyleValue::Type::Number);

	StyleValue ParseValue(Style::ValueInputVariant value, detail::StyleValueMultiMatchRule rule = StyleValue::Type::Number);
}

namespace s3d
{
	template<>
	inline FlexLayout::Style::StyleValue Parse<FlexLayout::Style::StyleValue>(const StringView s)
	{
		if (auto value = FlexLayout::Style::ParseValue(s))
		{
			return value;
		}

		detail::ThrowParseError("FlexLayout::Style::StyleValue", s);
	}

	inline Optional<FlexLayout::Style::StyleValue> ParseOpt(const StringView s)
	{
		if (auto value = FlexLayout::Style::ParseValue(s))
		{
			return value;
		}
		else
		{
			return none;
		}
	}
}
