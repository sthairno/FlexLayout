#pragma once
#include "../../Common.hpp"

namespace FlexLayout::Style::detail
{
	template<typename EnumType>
	struct style_enum_traits
	{
		static constexpr std::array<StringView, 0> names{ };
	};
}
