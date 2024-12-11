#pragma once
#include <type_traits>
#include <vector>
#include "../../Style/StyleValue.hpp"
#include "../../Style/StyleEnums.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	struct StyleValueMatchRule
	{
		template <class Enum, std::enable_if_t<std::is_scoped_enum_v<Enum>>* = nullptr>
		constexpr StyleValueMatchRule(Enum)
			: type(Style::StyleValue::Type::Enum)
			, enumTypeId(Style::detail::style_enum_id_from_type<Enum>::value)
		{ }

		constexpr StyleValueMatchRule(Style::EnumTypeId id)
			: type(Style::StyleValue::Type::Enum)
			, enumTypeId(id)
		{
			assert(id >= 0 && id < std::variant_size_v<Style::detail::style_enum_variant>);
		}

		constexpr StyleValueMatchRule(Style::StyleValue::Type t)
			: type(t)
			, enumTypeId(-1)
		{
			assert(t != Style::StyleValue::Type::Enum && t != Style::StyleValue::Type::Unspecified);
		}

		const Style::StyleValue::Type type;

		const Style::EnumTypeId enumTypeId;

		inline bool match(const Style::StyleValue& value) const noexcept
		{
			return value.type() == type &&
				(type != Style::StyleValue::Type::Enum || value.enumTypeId() == enumTypeId);
		}
	};

	struct StyleValueMultiMatchRule
	{
		constexpr StyleValueMultiMatchRule()
			: rules()
		{ }

		template <class Enum, std::enable_if_t<std::is_scoped_enum_v<Enum>>* = nullptr>
		constexpr StyleValueMultiMatchRule(Enum e)
			: rules({ StyleValueMatchRule{ e } })
		{ }

		constexpr StyleValueMultiMatchRule(Style::EnumTypeId id)
			: rules({ StyleValueMatchRule{ id } })
		{ }

		constexpr StyleValueMultiMatchRule(Style::StyleValue::Type t)
			: rules({ StyleValueMatchRule{ t } })
		{ }

		constexpr StyleValueMultiMatchRule(StyleValueMatchRule rule)
			: rules({ rule })
		{ };

		constexpr StyleValueMultiMatchRule(std::initializer_list<StyleValueMatchRule> rules)
			: rules(rules)
		{ };

		constexpr StyleValueMultiMatchRule(std::vector<StyleValueMatchRule> rules)
			: rules(rules)
		{ };

		const std::vector<StyleValueMatchRule> rules;

		constexpr StyleValueMultiMatchRule operator|(const StyleValueMatchRule& rule) const
		{
			std::vector<StyleValueMatchRule> newRules(rules);

			newRules.push_back(rule);

			return StyleValueMultiMatchRule{ newRules };
		}

		constexpr StyleValueMultiMatchRule operator|(const StyleValueMultiMatchRule& other) const
		{
			std::vector<StyleValueMatchRule> newRules(rules);

			newRules.append_range(other.rules);

			return StyleValueMultiMatchRule{ newRules };
		}

		template<class T>
		constexpr StyleValueMultiMatchRule operator|(T e) const
		{
			return *this | StyleValueMatchRule{ e };
		}

		inline bool match(const Style::StyleValue& value) const noexcept
		{
			return std::any_of(
				rules.begin(),
				rules.end(),
				[&](const StyleValueMatchRule& rule) { return rule.match(value); }
			);
		}
	};
}
