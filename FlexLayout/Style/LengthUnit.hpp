#pragma once
#include "../detail/Common.hpp"

namespace FlexLayout::Style
{
	/// @brief lengthプロパティの単位 | The unit of the length property
	enum class LengthUnit : int8_t
	{
		/// @brief ピクセル | Pixel
		Pixel = 0,

		/// @brief 未指定 | Unspecified
		Unspecified = Pixel,

		/// @brief "0"(U+0030)の送り幅 | The advance size of "0" (U+0030)
		Ch,

		/// @brief font-sizeの計算値 | The computed value of the font-size
		Em,

		/// @brief fontのxの高さ | The x-height of the font
		Ex,

		/// @brief "水"(U+6C34)の送り幅 | The advance size of "水" (U+6C34)
		Ic,

		/// @brief line-heightの計算値 | The computed value of the line-height
		Lh
	};
}
