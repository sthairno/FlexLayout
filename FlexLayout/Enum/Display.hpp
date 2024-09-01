#pragma once
#include "Common.hpp"

namespace FlexLayout
{
	enum class Display
	{
		Flex,
		None
	};

	template<>
	struct Style::detail::style_enum_traits<Display>
	{
		static constexpr std::array<StringView, 2> names{
			U"flex",
			U"none"
		};

		static constexpr std::array<YGDisplay, 8> to_yoga{
			YGDisplayFlex,
			YGDisplayNone
		};
	};
}
