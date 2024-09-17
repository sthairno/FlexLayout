#pragma once
#include "StylePropertyDefinition.hpp"

namespace FlexLayout::Internal
{
	struct StyleProperty
	{
		size_t keyHash;

		const StylePropertyDefinition* definition;

		Array<Style::StyleValue> value;

		bool removed;
	};

	enum class StylePropertyGroup : uint8
	{
		Preset = 0,
		StyleSheet,
		Inline
	};

	class StylePropertyTable
	{
	public:

		using value_type = StyleProperty;
		using container_type = Array<value_type>;

		const Array<value_type>& group(StylePropertyGroup group) const;

		value_type* get(StylePropertyGroup group, StringView key, size_t hash, bool moveToBack = false);

		template<class Key>
		inline value_type* get(StylePropertyGroup group, const Key& key, bool moveToBack = false)
		{
			return get(group, key, StylePropertyDefinitionList.hash(key), moveToBack);
		}

		value_type* find(StylePropertyGroup group, size_t hash);

		template<class Key>
		inline value_type* find(StylePropertyGroup group, const Key& key)
		{
			return find(group, StylePropertyDefinitionList.hash(key));
		}

	private:

		std::array<container_type, 3> m_table;
	};
}
