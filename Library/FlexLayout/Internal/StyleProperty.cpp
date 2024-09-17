#include "StyleProperty.hpp"

namespace FlexLayout::Internal
{
	const Array<StylePropertyTable::value_type>& StylePropertyTable::group(StylePropertyGroup group) const
	{
		return m_table[static_cast<uint8>(group)];
	}

	StylePropertyTable::value_type* StylePropertyTable::get(StylePropertyGroup group, StringView key, size_t hash, bool moveToBack)
	{
		auto& container = m_table[static_cast<uint8>(group)];

		auto itr = std::find_if(
			container.begin(),
			container.end(),
			[hash](const StyleProperty& item) { return item.keyHash == hash; }
		);

		if (itr == container.end())
		{
			auto defItr = StylePropertyDefinitionList.find(key, hash);

			if (defItr == StylePropertyDefinitionList.end())
			{
				return nullptr;
			}

			container.emplace_back(StyleProperty{
				.keyHash = hash,
				.definition = &defItr->second,
				.value = { },
				.removed = true
			});

			return &container.back();
		}

		if (moveToBack && container.end() - itr != 1)
		{
			StyleProperty tmp = std::move(*itr);
			container.erase(itr);
			container.emplace_back(std::move(tmp));

			return &container.back();
		}

		return &(*itr);
	}

	StylePropertyTable::value_type* StylePropertyTable::find(StylePropertyGroup group, size_t hash)
	{
		auto& container = m_table[static_cast<uint8>(group)];

		auto itr = std::find_if(
			container.begin(),
			container.end(),
			[hash](const StyleProperty& item) { return item.keyHash == hash; }
		);

		return itr == container.end()
			? nullptr
			: &(*itr);
	}
}
