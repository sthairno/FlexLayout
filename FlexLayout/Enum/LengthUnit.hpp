#pragma once
#include "../Common.hpp"

namespace FlexLayout
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

	void Formatter(FormatData& formatData, const LengthUnit& value);
}

template <>
struct SIV3D_HIDDEN fmt::formatter<FlexLayout::LengthUnit, s3d::char32>
{
	std::u32string tag;

	auto parse(fmt::basic_format_parse_context<s3d::char32>& ctx)
	{
		return s3d::detail::GetFormatTag(tag, ctx);
	}

	template <typename FormatContext>
	auto format(const FlexLayout::LengthUnit& value, FormatContext& ctx)
	{
		constexpr static std::u32string_view names[] = {
			U"px", U"ch", U"em", U"ex", U"ic", U"lh"
		};
		return format_to(ctx.out(), names[static_cast<int8_t>(value)]);
	}
};
