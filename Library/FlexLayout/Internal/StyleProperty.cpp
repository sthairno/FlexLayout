#include "StyleProperty.hpp"

namespace FlexLayout::Internal
{
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
				.definition = &(*defItr),
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

	const StylePropertyTable::value_type* StylePropertyTable::find(StylePropertyGroup group, size_t hash) const
	{
		auto& container = m_table[static_cast<uint8>(group)];

		auto itr = std::find_if(
			container.cbegin(),
			container.cend(),
			[hash](const StyleProperty& item) { return item.keyHash == hash; }
		);

		return itr == container.cend()
			? nullptr
			: &(*itr);
	}

	StylePropertyTable::value_type* StylePropertyTable::find(StylePropertyGroup group, size_t hash)
	{
		const auto& self = *this;
		return const_cast<value_type*>(self.find(group, hash));
	}

	const StylePropertyTable::value_type* StylePropertyTable::find(size_t hash) const
	{
		for (const auto& container : m_table)
		{
			auto itr = std::find_if(
				container.cbegin(),
				container.cend(),
				[hash](const StyleProperty& item) { return item.keyHash == hash; }
			);

			if (itr != container.cend())
			{
				return &(*itr);
			}
		}

		return nullptr;
	}
}
