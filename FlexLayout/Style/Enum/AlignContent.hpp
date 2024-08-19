#pragma once
#include "Common.hpp"

namespace FlexLayout::Style
{
	enum class AlignContent
	{
		Center,
		Start,
		End,
		SpaceBetween,
		SpaceAround,
		SpaceEvenly,
		Stretch,
	};

	namespace detail
	{
		template<>
		struct style_enum_traits<AlignContent>
		{
			static constexpr std::array<StringView, 7> names{
				U"center",
				U"start",
				U"end",
				U"space-between",
				U"space-around",
				U"space-evenly",
				U"stretch",
			};
		};
	}
}
