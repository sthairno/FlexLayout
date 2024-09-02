#pragma once
#include "Common.hpp"
#include "FlexBoxImpl.hpp"
#include "../Style/StyleValueMatchRule.hpp"

namespace FlexLayout::Internal
{
	using ApplyStyleCallback = std::function<bool(FlexBoxImpl&, std::span<const Style::StyleValue>)>;
	using ResetStyleCallback = std::function<bool(FlexBoxImpl&)>;

	struct StylePropertyDefinition
	{
		std::vector<std::vector<Style::detail::StyleValueMultiMatchRule>> patterns;

		ApplyStyleCallback applyCallback;

		ResetStyleCallback resetCallback;
	};

	extern const std::unordered_map<StringView, StylePropertyDefinition> StyleProperties;
}
