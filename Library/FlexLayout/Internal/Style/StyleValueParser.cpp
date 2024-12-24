#include "StyleValueParser.hpp"
#include <Siv3D/String.hpp>
#include <Siv3D/Indexed.hpp>

using namespace s3d;

namespace FlexLayout::Internal
{
	using Type = Style::StyleValue::Type;

	struct StyleValueParser
	{
		static Style::StyleValue ParseEnum(const String& str, Style::EnumTypeId enumId)
		{
			for (auto [idx, name] : Indexed(Style::detail::GetValueNameList(enumId)))
			{
				if (str == name)
				{
					return Style::StyleValue{
						Type::Enum,
						static_cast<std::int32_t>(idx),
						enumId
					};
				}
			}
			return { };
		}

		static Style::StyleValue ParseRatio(const String& str)
		{
			std::basic_istringstream<char32> stream{ str.data() };
			stream >> std::noskipws;

			float width;
			if (not(stream >> width))
			{
				return { };
			}

			stream >> std::ws;
			if (stream.eof())
			{
				return width >= 0.0f
					? Style::StyleValue{ Type::Ratio, width }
					: Style::StyleValue{ };
			}
			
			char32 maybeSlash;
			if (not(stream >> std::ws >> maybeSlash) || maybeSlash != U'/')
			{
				return { };
			}

			stream >> std::ws;
			float height;
			if (not(stream >> height))
			{
				return { };
			}

			return width >= 0.0F && height >= 0.0F
				? Style::StyleValue{ Type::Ratio, width / height }
				: Style::StyleValue{ };
		}

		static std::tuple<bool, float, String> ParseFloatSuffix(const String& str)
		{
			std::string buff = Unicode::ToUTF8(str);
			std::istringstream stream{ buff };
			stream >> std::noskipws;

			// `stream >> value`は数値のあとに'e'が続くと失敗するため、
			// std::strtof()を使って数値を読み取る
			// 
			// e.g. "1.0em"

			char* endptr = nullptr;
			float value = std::strtof(buff.data(), &endptr);
			if (errno == ERANGE || buff.data() == endptr ||
				not std::isfinite(value))
			{
				return { false, 0.0f, { } };
			}
			stream.ignore(endptr - buff.data());

			if (stream.eof())
			{
				// サフィックス無し
				return { true, value, { } };
			}

			std::string suffix;
			stream >> suffix;
			if (!stream.eof())
			{
				return { false, 0.0F, { } };
			}

			return { true, value, Unicode::FromUTF8(suffix) };
		}

		static bool ParseLengthUnit(const StringView suffix, LengthUnit& unit)
		{
			if (suffix.isEmpty())
			{
				unit = LengthUnit::Unspecified;
				return true;
			}
			else if (suffix == U"px")
			{
				unit = LengthUnit::Pixel;
				return true;
			}
			else if (suffix == U"em")
			{
				unit = LengthUnit::Em;
				return true;
			}
			else if (suffix == U"ex")
			{
				unit = LengthUnit::Ex;
				return true;
			}
			else if (suffix == U"ch")
			{
				unit = LengthUnit::Ch;
				return true;
			}
			else if (suffix == U"ic")
			{
				unit = LengthUnit::Ic;
				return true;
			}
			else if (suffix == U"lh")
			{
				unit = LengthUnit::Lh;
				return true;
			}
			return false;
		}

		static Style::StyleValue Parse(const String& str, StyleValueMatchRule rule)
		{
			switch (rule.type)
			{
			case Type::None:
			{
				if (str == U"none")
				{
					return Style::StyleValue{ Type::None };
				}
				break;
			}
			case Type::Auto:
			{
				if (str == U"auto")
				{
					return Style::StyleValue{ Type::Auto };
				}
				break;
			}
			case Type::Integer:
			{
				if (auto value = ParseOpt<int32_t>(str))
				{
					return Style::StyleValue{ Type::Integer, *value };
				}
				break;
			}
			case Type::Enum:
			{
				if (auto value = ParseEnum(str, rule.enumTypeId))
				{
					return value;
				}
				break;
			}
			case Type::Ratio:
			{
				if (auto value = ParseRatio(str))
				{
					return value;
				}
				break;
			}
			case Type::Percentage:
			{
				auto [success, value, suffix] = ParseFloatSuffix(str);
				if (success && suffix == U"%")
				{
					return Style::StyleValue{ Type::Percentage, value };
				}
				break;
			}
			case Type::Number:
			{
				if (auto value = ParseOpt<float>(str))
				{
					return Style::StyleValue{ Type::Number, *value };
				}
				break;
			}
			case Type::Length:
			{
				auto [success, value, suffix] = ParseFloatSuffix(str);
				if (success && value >= 0.0F)
				{
					LengthUnit unit;
					if (ParseLengthUnit(suffix, unit))
					{
						return Style::StyleValue{ Type::Length, value, unit };
					}
				}
				break;
			}
			}

			return { };
		}

		static Style::StyleValue Parse(std::int32_t value, StyleValueMatchRule rule)
		{
			switch (rule.type)
			{
			case Type::Integer:
				return Style::StyleValue{ Type::Integer, value };
			case Type::Ratio:
				if (value < 0)
				{
					break;
				}
				return Style::StyleValue{ Type::Ratio, static_cast<float>(value) };
			case Type::Percentage:
				return Style::StyleValue{ Type::Percentage, static_cast<float>(value) };
			case Type::Number:
				return Style::StyleValue{ Type::Number, static_cast<float>(value) };
			case Type::Length:
				if (value < 0)
				{
					break;
				}
				return Style::StyleValue{ Type::Length, static_cast<float>(value) };
			}

			return { };
		}
		
		static Style::StyleValue Parse(float value, StyleValueMatchRule rule)
		{
			switch (rule.type)
			{
			case Type::Ratio:
				if (not std::isfinite(value) || value < 0.0F)
				{
					break;
				}
				return Style::StyleValue{ Type::Ratio, value };
			case Type::Percentage:
				if (not std::isfinite(value))
				{
					break;
				}
				return Style::StyleValue{ Type::Percentage, value };
			case Type::Number:
				if (not std::isfinite(value))
				{
					break;
				}
				return Style::StyleValue{ Type::Number, value };
			case Type::Length:
				if (not std::isfinite(value) || value < 0.0F)
				{
					break;
				}
				return Style::StyleValue{ Type::Length, value };
			}

			return { };
		}
	};
	

	Style::StyleValue ParseValue(std::int32_t src, StyleValueMultiMatchRule rules)
	{
		for (auto& rule : rules.rules)
		{
			if (auto value = StyleValueParser::Parse(src, rule))
			{
				return value;
			}
		}

		return { };
	}

	Style::StyleValue ParseValue(float src, StyleValueMultiMatchRule rules)
	{
		for (auto& rule : rules.rules)
		{
			if (auto value = StyleValueParser::Parse(src, rule))
			{
				return value;
			}
		}

		return { };
	}

	Style::StyleValue ParseValue(const StringView str, StyleValueMultiMatchRule rules)
	{
		String trimmedStr{ str };
		trimmedStr.trim();

		for (auto& rule : rules.rules)
		{
			if (auto value = StyleValueParser::Parse(trimmedStr, rule))
			{
				return value;
			}
		}

		return { };
	}

	struct parser_visitor
	{
		StyleValueMultiMatchRule rules;

		Style::StyleValue operator()(const Style::StyleValue& value)
		{
			return rules.match(value)
				? value
				: Style::StyleValue{ };
		}

		Style::StyleValue operator()(std::int32_t value)
		{
			return ParseValue(value, rules);
		}

		Style::StyleValue operator()(float value)
		{
			return ParseValue(value, rules);
		}

		Style::StyleValue operator()(const StringView value)
		{
			return ParseValue(value, rules);
		}

		template<class Enum>
		Style::StyleValue operator()(Enum value)
		{
			for (auto& rule : rules.rules)
			{
				if (rule.type == Style::StyleValue::Type::Enum &&
					rule.enumTypeId == Style::detail::style_enum_id_from_type<Enum>::value)
				{
					return Style::StyleValue::Enum(value);
				}
			}

			return { };
		}
	};

	Style::StyleValue ParseValue(Style::ValueInputVariant value, StyleValueMultiMatchRule rule)
	{
		return std::visit(parser_visitor{ rule }, value);
	}
}
