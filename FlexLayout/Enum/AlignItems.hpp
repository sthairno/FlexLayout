#pragma once
#include "Common.hpp"

namespace FlexLayout
{
	enum class AlignItems
	{
		Center,
		FlexStart,
		FlexEnd,
		Start,
		End,
		Stretch,
		Baseline
	};

	template<>
	struct Style::detail::style_enum_traits<AlignItems>
	{
		static constexpr std::array<s3d::StringView, 7> names{
			U"center",
			U"flex-start",
			U"flex-end",
			U"start",
			U"end",
			U"stretch",
			U"baseline"
		};

		static constexpr std::array<YGAlign, 7> to_yoga{
			YGAlignCenter,
			YGAlignFlexStart,
			YGAlignFlexEnd,
			YGAlignFlexStart,
			YGAlignFlexEnd,
			YGAlignStretch,
			YGAlignBaseline
		};
	};
}
