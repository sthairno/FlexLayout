#pragma once
#include "Common.hpp"

namespace FlexLayout
{
	enum class FlexWrap
	{
		NoWrap,
		Wrap,
		WrapReverse
	};

	template<>
	struct Style::detail::style_enum_traits<FlexWrap>
	{
		static constexpr std::array<s3d::StringView, 3> names{
			U"nowrap",
			U"wrap",
			U"wrap-reverse"
		};

		static constexpr std::array<YGWrap, 3> to_yoga{
			YGWrapNoWrap,
			YGWrapWrap,
			YGWrapWrapReverse
		};
	};
}
