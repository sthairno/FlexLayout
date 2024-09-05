#include "StyleValueParser.hpp"

namespace FlexLayout::Style
{
	using Type = StyleValue::Type;

	struct detail::StyleValueParser
	{
		static StyleValue ParseEnum(const String& str, EnumTypeId enumId)
		{
			for (auto [idx, name] : Indexed(detail::GetValueNameList(enumId)))
			{
				if (str == name)
				{
					return StyleValue{
						Type::Enum,
						static_cast<std::int32_t>(idx),
						enumId
					};
				}
			}
			return { };
		}

		static StyleValue ParseRatio(const String& str)
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
					? StyleValue{ Type::Ratio, width }
					: StyleValue{ };
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
				? StyleValue{ Type::Ratio, width / height }
				: StyleValue{ };
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

			stream >> std::ws;
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

		static StyleValue Parse(const String& str, detail::StyleValueMatchRule rule)
		{
			switch (rule.type)
			{
			case Type::None:
			{
				if (str == U"none")
				{
					return StyleValue{ Type::None };
				}
				break;
			}
			case Type::Auto:
			{
				if (str == U"auto")
				{
					return StyleValue{ Type::Auto };
				}
				break;
			}
			case Type::Integer:
			{
				if (auto value = ParseOpt<int32_t>(str))
				{
					return StyleValue{ Type::Integer, *value };
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
					return StyleValue{ Type::Percentage, value };
				}
				break;
			}
			case Type::Number:
			{
				if (auto value = ParseOpt<float>(str))
				{
					return StyleValue{ Type::Number, *value };
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
						return StyleValue{ Type::Length, value, unit };
					}
				}
				break;
			}
			}

			return { };
		}

		static StyleValue Parse(std::int32_t value, detail::StyleValueMatchRule rule)
		{
			switch (rule.type)
			{
			case Type::Integer:
				return StyleValue{ Type::Integer, value };
			case Type::Ratio:
				if (value < 0)
				{
					break;
				}
				return StyleValue{ Type::Ratio, static_cast<float>(value) };
			case Type::Percentage:
				return StyleValue{ Type::Percentage, static_cast<float>(value) };
			case Type::Number:
				return StyleValue{ Type::Number, static_cast<float>(value) };
			case Type::Length:
				if (value < 0)
				{
					break;
				}
				return StyleValue{ Type::Length, static_cast<float>(value) };
			}

			return { };
		}
		
		static StyleValue Parse(float value, detail::StyleValueMatchRule rule)
		{
			switch (rule.type)
			{
			case Type::Ratio:
				if (not std::isfinite(value) || value < 0.0F)
				{
					break;
				}
				return StyleValue{ Type::Ratio, value };
			case Type::Percentage:
				if (not std::isfinite(value))
				{
					break;
				}
				return StyleValue{ Type::Percentage, value };
			case Type::Number:
				if (not std::isfinite(value))
				{
					break;
				}
				return StyleValue{ Type::Number, value };
			case Type::Length:
				if (not std::isfinite(value) || value < 0.0F)
				{
					break;
				}
				return StyleValue{ Type::Length, value };
			}

			return { };
		}
	};
	

	StyleValue ParseValue(std::int32_t src, detail::StyleValueMultiMatchRule rules)
	{
		for (auto& rule : rules.rules)
		{
			if (auto value = detail::StyleValueParser::Parse(src, rule))
			{
				return value;
			}
		}

		return { };
	}

	StyleValue ParseValue(float src, detail::StyleValueMultiMatchRule rules)
	{
		for (auto& rule : rules.rules)
		{
			if (auto value = detail::StyleValueParser::Parse(src, rule))
			{
				return value;
			}
		}

		return { };
	}

	StyleValue ParseValue(const StringView str, detail::StyleValueMultiMatchRule rules)
	{
		String trimmedStr{ str };
		trimmedStr.trim();

		for (auto& rule : rules.rules)
		{
			if (auto value = detail::StyleValueParser::Parse(trimmedStr, rule))
			{
				return value;
			}
		}

		return { };
	}
}
