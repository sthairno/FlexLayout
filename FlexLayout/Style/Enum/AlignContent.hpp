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
			static constexpr StyleEnumTypeID id = StyleEnumTypeID::AlignContent;
		};
	}
}
