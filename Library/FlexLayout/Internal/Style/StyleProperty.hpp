#pragma once
#include "StylePropertyDefinition.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	class StyleProperty
	{
	public:

		enum class Event
		{
			None,
			Created,
			Updated,
			Removed
		};

		StyleProperty(StyleProperty&&) = default;

		StyleProperty& operator =(StyleProperty&&) = default;

		StyleProperty(const StyleProperty&) = delete;

		StyleProperty& operator =(const StyleProperty&) = delete;

		inline size_t keyHash() const { return m_keyHash; }

		inline const StylePropertyDefinitionRef& definition() const { return m_definition; }

		inline bool execInstall(FlexBoxNode& impl) const { return m_definition.installCallback(impl, m_value); }

		inline void execReset(FlexBoxNode& impl) const { m_definition.resetCallback(impl); }

		inline bool removed() const { return m_value.empty(); }

		inline const Array<Style::StyleValue>& value() const { return m_value; }

		inline void setValue(Array<Style::StyleValue>&& newValue)
		{
			assert(newValue);

			if (m_value == newValue)
			{
				return;
			}

			switch (m_event)
			{
			case Event::None: m_event = m_value ? Event::Updated : Event::Created; break;
			case Event::Created: break;
			case Event::Updated: break;
			case Event::Removed: m_event = Event::Updated; break;
			}

			m_value = std::move(newValue);
		}

		inline void unsetValue()
		{
			if (m_value.empty())
			{
				return;
			}

			switch (m_event)
			{
			case Event::None: m_event = Event::Removed; break;
			case Event::Created: m_event = Event::None; break;
			case Event::Updated: m_event = Event::Removed; break;
			}

			m_value.clear();
		}

		inline Event event() const { return m_event; }

		inline void clearEvent() { m_event = Event::None; }

		inline static size_t Hash(StringView key) { return StylePropertyDefinitionList.hash(key); }

	protected:

		friend class StylePropertyTable;

		StyleProperty(size_t keyHash, decltype(StylePropertyDefinitionList)::const_pointer definition)
			: m_keyHash(keyHash)
			, m_definition(definition)
			, m_value()
			, m_event(Event::None) { }

	private:

		size_t m_keyHash;

		StylePropertyDefinitionRef m_definition;

		Array<Style::StyleValue> m_value;

		Event m_event;
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

		value_type* find(size_t hash);

		template<class Key>
		inline value_type* find(const Key& key)
		{
			return find(StyleProperty::Hash(key));
		}

		const value_type* find(size_t hash) const;

		template<class Key>
		inline const value_type* find(const Key& key) const
		{
			return find(StyleProperty::Hash(key));
		}

		constexpr size_t size() const noexcept { return m_table.size(); }

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
