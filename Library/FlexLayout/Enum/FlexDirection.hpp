﻿#pragma once
#include "Common.hpp"

namespace FlexLayout
{
	enum class FlexDirection
	{
		Row,
		RowReverse,
		Column,
		ColumnReverse
	};

	template<>
	struct Style::detail::style_enum_traits<FlexDirection>
	{
		static constexpr std::array<s3d::StringView, 4> names{
			U"row",
			U"row-reverse",
			U"column",
			U"column-reverse"
		};

#if FLEXLAYOUT_ENABLE_CONV_TO_YOGA
		static constexpr std::array<YGFlexDirection, 4> to_yoga{
			YGFlexDirectionRow,
			YGFlexDirectionRowReverse,
			YGFlexDirectionColumn,
			YGFlexDirectionColumnReverse
		};
#endif
	};
}
