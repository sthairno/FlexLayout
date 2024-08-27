#pragma once
#include "Common.hpp"

namespace FlexLayout
{
	enum class TextAlign
	{
		Start,
		End,
		Left,
		Right,
		Center
	};

	template<>
	struct Style::detail::style_enum_traits<TextAlign>
	{
		static constexpr std::array<StringView, 7> names{
			U"start",
			U"end",
			U"left",
			U"right",
			U"center"
		};
	};
}
