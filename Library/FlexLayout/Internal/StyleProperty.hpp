#pragma once
#include "StylePropertyDefinition.hpp"

namespace FlexLayout::Internal
{
	struct StyleProperty
	{
		size_t keyHash;

		decltype(StylePropertyDefinitionList)::const_pointer definition;

		Array<Style::StyleValue> value;

		bool removed;

		inline const StringView name() const
		{
			return definition->first;
		}

		inline bool install(FlexBoxImpl& impl) const
		{
			return definition->second.installCallback(impl, value);
		}

		inline void reset(FlexBoxImpl& impl) const
		{
			definition->second.resetCallback(impl);
		}
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
		using group_container_type = Array<value_type>;
		using container_type = std::array<group_container_type, 3>;

		inline std::span<value_type> group(StylePropertyGroup group)
		{
			// 配列の内容の編集は許可するが、配列そのものの操作は許可したくない

			auto& ary = m_table[static_cast<uint8>(group)];
			return std::span{ ary.begin(), ary.end() };
		}

		inline const group_container_type& group(StylePropertyGroup group) const
		{
			return m_table[static_cast<uint8>(group)];
		}

		value_type* get(StylePropertyGroup group, StringView key, size_t hash, bool moveToBack = false);

		template<class Key>
		inline value_type* get(StylePropertyGroup group, const Key& key, bool moveToBack = false)
		{
			return get(group, key, Hash(key), moveToBack);
		}

		const value_type* find(StylePropertyGroup group, size_t hash) const;

		template<class Key>
		inline const value_type* find(StylePropertyGroup group, const Key& key) const
		{
			return find(group, Hash(key));
		}

		value_type* find(StylePropertyGroup group, size_t hash);

		template<class Key>
		inline value_type* find(StylePropertyGroup group, const Key& key)
		{
			return find(group, Hash(key));
		}

		const value_type* find(size_t hash) const;

		template<class Key>
		inline const value_type* find(const Key& key) const
		{
			return find(Hash(key));
		}

		constexpr container_type::iterator begin() noexcept { return m_table.begin(); }

		constexpr container_type::const_iterator begin() const noexcept { return m_table.cbegin(); }
		
		constexpr container_type::iterator end() noexcept { return m_table.end(); }

		constexpr container_type::const_iterator end() const noexcept { return m_table.cend(); }

		constexpr container_type::reverse_iterator rbegin() noexcept { return m_table.rbegin(); }

		constexpr container_type::const_reverse_iterator rbegin() const noexcept { return m_table.crbegin(); }

		constexpr container_type::reverse_iterator rend() noexcept { return m_table.rend(); }

		constexpr container_type::const_reverse_iterator rend() const noexcept { return m_table.crend(); }

		constexpr container_type::const_iterator cbegin() const noexcept { return m_table.cbegin(); }

		constexpr container_type::const_iterator cend() const noexcept { return m_table.cend(); }

		constexpr container_type::const_reverse_iterator crbegin() const noexcept { return m_table.crbegin(); }

		constexpr container_type::const_reverse_iterator crend() const noexcept { return m_table.crend(); }

		static size_t Hash(StringView key) noexcept
		{
			return StylePropertyDefinitionList.hash(key);
		}

	private:

		container_type m_table;
	};
}
