﻿#pragma once
#include "Common.hpp"

namespace FlexLayout
{
	enum class AlignSelf
	{
		Auto,
		Center,
		FlexStart,
		FlexEnd,
		Start,
		End,
		Stretch,
		Baseline
	};

	template<>
	struct Style::detail::style_enum_traits<AlignSelf>
	{
		static constexpr std::array<s3d::StringView, 8> names{
			U"auto",
			U"center",
			U"flex-start",
			U"flex-end",
			U"start",
			U"end",
			U"stretch",
			U"baseline"
		};

#if FLEXLAYOUT_ENABLE_CONV_TO_YOGA
		static constexpr std::array<YGAlign, 8> to_yoga{
			YGAlignAuto,
			YGAlignCenter,
			YGAlignFlexStart,
			YGAlignFlexEnd,
			YGAlignFlexStart,
			YGAlignFlexEnd,
			YGAlignStretch,
			YGAlignBaseline
		};
#endif
	};
}
