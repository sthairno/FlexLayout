#pragma once
#include "Common.hpp"

namespace FlexLayout
{
	enum class JustifyContent
	{
		Center,
		Start,
		End,
		FlexStart,
		FlexEnd,
		SpaceBetween,
		SpaceAround,
		SpaceEvenly
	};

	template<>
	struct Style::detail::style_enum_traits<JustifyContent>
	{
		static constexpr std::array<s3d::StringView, 8> names{
			U"center",
			U"start",
			U"end",
			U"flex-start",
			U"flex-end",
			U"space-between",
			U"space-around",
			U"space-evenly"
		};

		static constexpr std::array<YGJustify, 8> to_yoga{
			YGJustifyCenter,
			YGJustifyFlexStart,
			YGJustifyFlexEnd,
			YGJustifyFlexStart,
			YGJustifyFlexEnd,
			YGJustifySpaceBetween,
			YGJustifySpaceAround,
			YGJustifySpaceEvenly
		};
	};
}
