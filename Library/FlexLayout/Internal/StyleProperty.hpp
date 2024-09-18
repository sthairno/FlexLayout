#pragma once
#include "StylePropertyDefinition.hpp"

namespace FlexLayout::Internal
{
	class StyleProperty
	{
	public:

		StyleProperty(StyleProperty&&) = default;

		StyleProperty& operator =(StyleProperty&&) = default;

		StyleProperty(const StyleProperty&) = delete;

		StyleProperty& operator =(const StyleProperty&) = delete;

		Array<Style::StyleValue> value;

		inline const StringView name() const { return m_definition->first; }

		inline size_t keyHash() const { return m_keyHash; }

		inline const StylePropertyDefinition& definition() const { return m_definition->second; }

		inline auto& patterns() const { return m_definition->second.patterns; }

		inline bool install(FlexBoxImpl& impl) const { return m_definition->second.installCallback(impl, value); }

		inline void reset(FlexBoxImpl& impl) const { m_definition->second.resetCallback(impl); }

		inline bool removed() const { return value.empty(); }

		inline void clear() { value.clear(); }

		inline static size_t Hash(StringView key) { return StylePropertyDefinitionList.hash(key); }

	protected:

		friend class StylePropertyTable;

		StyleProperty(size_t keyHash, decltype(StylePropertyDefinitionList)::const_pointer definition)
			: m_keyHash(keyHash)
			, m_definition(definition)
			, value() { }

	private:

		size_t m_keyHash;

		decltype(StylePropertyDefinitionList)::const_pointer m_definition;
	};

	/// @brief スタイルのグループ
	/// @remark 数値が大きいほど優先度が高い
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

		/// @brief グループごとのプロパティを格納するコンテナ
		/// @remark インデックスが大きいほど優先度が高い。コンテナ内は同じプロパティが重複しないことが保証される。
		using group_container_type = Array<value_type>;

		using group_span_type = std::span<value_type>;

		using container_type = std::array<group_container_type, 3>;

		inline group_span_type group(StylePropertyGroup group)
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
			return get(group, key, StyleProperty::Hash(key), moveToBack);
		}

		const value_type* find(StylePropertyGroup group, size_t hash) const;

		template<class Key>
		inline const value_type* find(StylePropertyGroup group, const Key& key) const
		{
			return find(group, StyleProperty::Hash(key));
		}

		value_type* find(StylePropertyGroup group, size_t hash);

		template<class Key>
		inline value_type* find(StylePropertyGroup group, const Key& key)
		{
			return find(group, StyleProperty::Hash(key));
		}

		const value_type* find(size_t hash) const;

		template<class Key>
		inline const value_type* find(const Key& key) const
		{
			return find(StyleProperty::Hash(key));
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

	private:

		container_type m_table;
	};
}
