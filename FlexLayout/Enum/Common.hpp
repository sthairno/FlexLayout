#pragma once
#include "../Common.hpp"
#include <yoga/YGEnums.h>

namespace FlexLayout::Style::detail
{
	template<typename EnumType>
	struct style_enum_traits
	{
		static constexpr std::array<StringView, 0> names{ };

		static constexpr std::array<int, 0> to_yoga;
	};
}
