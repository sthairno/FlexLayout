#pragma once
#include <array>
#include <Siv3D/StringView.hpp>

#if defined YG_ENUM_DECL
	#define FLEXLAYOUT_ENABLE_CONV_TO_YOGA 1
#else
	#define FLEXLAYOUT_ENABLE_CONV_TO_YOGA 0
#endif

namespace FlexLayout::Style::detail
{
	template<typename EnumType>
	struct style_enum_traits
	{
		static constexpr std::array<s3d::StringView, 0> names{ };

#if FLEXLAYOUT_ENABLE_CONV_TO_YOGA
		static constexpr std::array<int, 0> to_yoga;
#endif
	};
}
