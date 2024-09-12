#include <Siv3D/StringView.hpp>
#include <ThirdParty/fmt/format.h>
#include "StyleEnums.hpp"

using namespace s3d;

namespace FlexLayout::Style::detail
{
	template <EnumTypeId ID>
	static std::span<const StringView> SearchRecursive(EnumTypeId enumId)
	{
		if constexpr (ID < std::variant_size_v<style_enum_variant>)
		{
			if (enumId == ID)
			{
				return style_enum_traits_by_id<ID>::names;
			}
			else
			{
				return SearchRecursive<ID + 1>(enumId);
			}
		}
		else
		{
			return { };
		}
	}

	std::span<const StringView> GetValueNameList(EnumTypeId enumId)
	{
		if (enumId < 0 || enumId >= std::variant_size_v<style_enum_variant>)
		{
			throw std::invalid_argument(fmt::format(
				"Invalid EnumTypeId => {} (expected: 0-{})",
				enumId, std::variant_size_v<style_enum_variant> -1
			));
		}

		return SearchRecursive<0>(enumId);
	}

	const StringView GetValueName(EnumTypeId enumId, std::int32_t value)
	{
		auto list = GetValueNameList(enumId);

		if (list.empty())
		{
			throw std::runtime_error("Failed to get enum name list");
		}

		if (value < 0 || value >= list.size())
		{
			throw std::out_of_range(fmt::format("Invalid value => {} (expected: 0-{})", enumId, list.size()));
		}

		return list[value];
	}
}
