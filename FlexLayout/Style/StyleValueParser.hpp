#include "../detail/Common.hpp"
#include "StyleValue.hpp"

namespace FlexLayout::Style
{
	namespace detail
	{
		struct StyleValueMatchRule
		{
			template <class Enum, std::enable_if_t<std::is_scoped_enum_v<Enum>>* = nullptr>
			StyleValueMatchRule(const Enum& e)
				: type(StyleValue::Type::Enum)
				, enumTypeId(style_enum_id_from_type<Enum>::value)
			{ }

			StyleValueMatchRule(const EnumTypeId id)
				: type(StyleValue::Type::Enum)
				, enumTypeId(id)
			{
				assert(id >= 0 && id < std::variant_size_v<style_enum_variant>);
			}

			StyleValueMatchRule(StyleValue::Type t)
				: type(t)
				, enumTypeId(-1)
			{
				assert(t != StyleValue::Type::Enum && t != StyleValue::Type::Unspecified);
			}

			const StyleValue::Type type;

			const EnumTypeId enumTypeId;
		};
	}

	StyleValue ParseValue(const StringView str, Array<detail::StyleValueMatchRule> rules);

	inline StyleValue ParseValue(const StringView str, detail::StyleValueMatchRule rule = StyleValue::Type::Number)
	{
		return ParseValue(str, Array{ rule });
	}
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
