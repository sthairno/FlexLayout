// 列挙型の追加方法
// 1. ディレクトリ Enum/ に新しいヘッダファイルを追加
// 3. 新しいファイル内にenumとstyle_enum_traitsを実装
// 4. このファイルにincludeとenumを追加

// vvvvv--include--vvvvv

#include "Enum/Common.hpp"
#include "Enum/AlignContent.hpp"
// #include "Enum/[.....].hpp"

// ^^^^^----------^^^^^

namespace FlexLayout::Style::detail
{
	using style_enum_variant = std::variant<
		// vvvvv---enum---vvvvv

		AlignContent
		// [.....],

		// ^^^^^----------^^^^^
	>;
}

////////////////////////////////////////////////////

namespace FlexLayout::Style
{
	using EnumTypeId = std::int8_t;

	namespace detail
	{
		template <typename T>
		struct style_enum_id_from_type
			: std::integral_constant<EnumTypeId, style_enum_variant(T()).index()>
		{ };

		template <EnumTypeId ID>
		using style_enum_by_id = std::variant_alternative_t<size_t(ID), style_enum_variant>;

		template <EnumTypeId ID>
		using style_enum_traits_by_id = style_enum_traits<style_enum_by_id<ID>>;

		std::span<const StringView> GetValueNameList(EnumTypeId enumId);

		const StringView GetValueName(EnumTypeId enumId, std::int32_t value);

		template <class Enum>
		inline const StringView GetValueName(Enum value)
		{
			return GetValueName(style_enum_id_from_type<Enum>::value, static_cast<std::int32_t>(value));
		}
	}
}
