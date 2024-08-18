#pragma once
#include "../../detail/Common.hpp"

namespace FlexLayout::Style
{
	enum class StyleEnumTypeID : int8_t
	{
		Unknown = 0,
		AlignContent,
	};

	namespace detail
	{
		template<typename EnumType>
		struct style_enum_traits
		{
			static constexpr StyleEnumTypeID id = StyleEnumTypeID::Unknown;
		};
	}
}
