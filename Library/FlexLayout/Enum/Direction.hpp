#pragma once
#include "Common.hpp"

namespace FlexLayout
{
	enum class Direction
	{
		LTR,
		RTL
	};

	template<>
	struct Style::detail::style_enum_traits<Direction>
	{
		static constexpr std::array<s3d::StringView, 8> names{
			U"ltr",
			U"rtl"
		};

		static constexpr std::array<YGDirection, 8> to_yoga{
			YGDirectionLTR,
			YGDirectionRTL
		};
	};
}
