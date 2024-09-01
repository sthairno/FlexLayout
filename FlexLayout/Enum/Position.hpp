#pragma once
#include "Common.hpp"

namespace FlexLayout
{
	enum class Position
	{
		Static,
		Relative,
		Absolute
	};

	template<>
	struct Style::detail::style_enum_traits<Position>
	{
		static constexpr std::array<StringView, 3> names{
			U"static",
			U"relative",
			U"absolute"
		};

		static constexpr std::array<YGPositionType, 3> to_yoga{
			YGPositionTypeStatic,
			YGPositionTypeRelative,
			YGPositionTypeAbsolute
		};
	};
}
