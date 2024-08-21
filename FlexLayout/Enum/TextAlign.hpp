#pragma once
#include "Common.hpp"

namespace FlexLayout
{
	enum class TextAlign
	{
		Left,
		Right,
		Center
	};

	template<>
	struct Style::detail::style_enum_traits<TextAlign>
	{
		static constexpr std::array<StringView, 7> names{
			U"left",
			U"right",
			U"center"
		};
	};
}
