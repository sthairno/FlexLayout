#pragma once
#include "../Common.hpp"
#include "StyleValue.hpp"

namespace FlexLayout::Style::detail
{
	struct StyleValueMatchRule
	{
		template <class Enum, std::enable_if_t<std::is_scoped_enum_v<Enum>>* = nullptr>
		constexpr StyleValueMatchRule(Enum)
			: type(StyleValue::Type::Enum)
			, enumTypeId(style_enum_id_from_type<Enum>::value)
		{ }

		constexpr StyleValueMatchRule(EnumTypeId id)
			: type(StyleValue::Type::Enum)
			, enumTypeId(id)
		{
			assert(id >= 0 && id < std::variant_size_v<style_enum_variant>);
		}

		constexpr StyleValueMatchRule(StyleValue::Type t)
			: type(t)
			, enumTypeId(-1)
		{
			assert(t != StyleValue::Type::Enum && t != StyleValue::Type::Unspecified);
		}

		const StyleValue::Type type;

		const EnumTypeId enumTypeId;

		inline bool match(const StyleValue& value) const noexcept
		{
			return value.type() == type &&
				(type != StyleValue::Type::Enum || value.enumTypeId() == enumTypeId);
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

		constexpr StyleValueMultiMatchRule(EnumTypeId id)
			: rules({ StyleValueMatchRule{ id } })
		{ }

		constexpr StyleValueMultiMatchRule(StyleValue::Type t)
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

		inline bool match(const StyleValue& value) const noexcept
		{
			return std::any_of(
				rules.begin(),
				rules.end(),
				[&](const StyleValueMatchRule& rule) { return rule.match(value); }
			);
		}
	};
}
