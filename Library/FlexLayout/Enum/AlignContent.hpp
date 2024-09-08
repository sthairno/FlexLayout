#pragma once
#include "Common.hpp"

namespace FlexLayout
{
	enum class AlignContent
	{
		Center,
		FlexStart,
		FlexEnd,
		Start,
		End,
		SpaceBetween,
		SpaceAround,
		SpaceEvenly,
		Stretch,
	};

	template<>
	struct Style::detail::style_enum_traits<AlignContent>
	{
		static constexpr std::array<s3d::StringView, 9> names{
			U"center",
			U"flex-start",
			U"flex-end",
			U"start",
			U"end",
			U"space-between",
			U"space-around",
			U"space-evenly",
			U"stretch",
		};

#if FLEXLAYOUT_ENABLE_CONV_TO_YOGA
		static constexpr std::array<YGAlign, 9> to_yoga{
			YGAlignCenter,
			YGAlignFlexStart,
			YGAlignFlexEnd,
			YGAlignFlexStart,
			YGAlignFlexEnd,
			YGAlignSpaceBetween,
			YGAlignSpaceAround,
			YGAlignSpaceEvenly,
			YGAlignStretch
		};
#endif
	};
}
