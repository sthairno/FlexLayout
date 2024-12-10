#pragma once
#include <Siv3D/Parse.hpp>
#include "StyleValueMatchRule.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	Style::StyleValue ParseValue(std::int32_t value, StyleValueMultiMatchRule rule = Style::StyleValue::Type::Number);

	Style::StyleValue ParseValue(float value, StyleValueMultiMatchRule rule = Style::StyleValue::Type::Number);

	Style::StyleValue ParseValue(const s3d::StringView str, StyleValueMultiMatchRule rule = Style::StyleValue::Type::Number);

	Style::StyleValue ParseValue(Style::ValueInputVariant value, StyleValueMultiMatchRule rule = Style::StyleValue::Type::Number);
}
//
//namespace s3d
//{
//	template<>
//	inline Style::StyleValue Parse<Style::StyleValue>(const s3d::StringView s)
//	{
//		if (auto value = ::FlexLayout::Internal::ParseValue(s))
//		{
//			return value;
//		}
//
//		detail::ThrowParseError("FlexLayout::Style::StyleValue", s);
//	}
//
//	inline Optional<FlexLayout::Style::StyleValue> ParseOpt(const s3d::StringView s)
//	{
//		if (auto value = FlexLayout::Style::ParseValue(s))
//		{
//			return value;
//		}
//		else
//		{
//			return none;
//		}
//	}
//}
