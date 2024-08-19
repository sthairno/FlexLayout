#include "StyleValueParser.hpp"

namespace FlexLayout::Style
{
	using Type = StyleValue::Type;

	struct detail::Parser
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
			std::basic_istringstream<char32> stream{ str.data() };
			stream >> std::noskipws;

			float value;
			if (not(stream >> value))
			{
				return { false, 0.0f, { } };
			}

			stream >> std::ws;
			if (stream.eof())
			{
				return { true, value, { } };
			}

			std::u32string suffix;
			stream >> suffix;
			if (!stream.eof())
			{
				return { false, 0.0F, { } };
			}

			return { true, value, { suffix } };
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
	};
	

	StyleValue ParseValue(const StringView str, Array<detail::StyleValueMatchRule> rules)
	{
		String trimmedStr{ str };
		trimmedStr.trim();

		for (auto& rule : rules)
		{
			if (auto value = detail::Parser::Parse(trimmedStr, rule))
			{
				return value;
			}
		}

		return { };
	}
}
