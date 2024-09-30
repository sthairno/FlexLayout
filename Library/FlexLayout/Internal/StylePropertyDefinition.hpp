#pragma once
#include <functional>
#include <vector>
#include <Siv3D/StringView.hpp>
#include <Siv3D/HeterogeneousLookupHelper.hpp>
#include <Siv3D/HashTable.hpp>
#include "../Style/StyleValueMatchRule.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	class FlexBoxImpl;

	using StyleInstallCallback = std::function<bool(FlexBoxImpl&, std::span<const Style::StyleValue>)>;
	using StyleResetCallback = std::function<void(FlexBoxImpl&)>;

	struct StylePropertyDefinitionDetails
	{
		/// @brief 受け付ける入力値のパターン
		const std::vector<std::vector<Style::detail::StyleValueMultiMatchRule>> patterns;

		/// @brief 入力値をFlexBoxImplに反映させるコールバック
		const StyleInstallCallback installCallback;

		/// @brief プロパティを初期設定に戻すコールバック
		const StyleResetCallback resetCallback;

		/// @brief installCallback/resetCallback呼び出し時に影響を与える(可能性のある)プロパティ名
		const std::vector<StringView> maybeAffectTo;
	};

	using StylePropertyDefinitionContainer = HashTable<s3d::StringView, StylePropertyDefinitionDetails>;

	class StylePropertyDefinitionRef
	{
	public:

		StylePropertyDefinitionRef(StylePropertyDefinitionContainer::const_iterator itr)
			: m_name(itr->first)
			, m_details(&itr->second)
		{ }

		StylePropertyDefinitionRef(StylePropertyDefinitionContainer::const_pointer ptr)
			: m_name(ptr->first)
			, m_details(&ptr->second)
		{ }

		inline const StringView name() const { return m_name; }

		inline const auto& patterns() const { return m_details->patterns; }

		inline bool installCallback(FlexBoxImpl& impl, std::span<const Style::StyleValue> values) const { return m_details->installCallback(impl, values); }

		inline void resetCallback(FlexBoxImpl& impl) const { return m_details->resetCallback(impl); }

		inline const auto& maybeAffectTo() const { return m_details->maybeAffectTo; }

	private:

		StringView m_name;

		const StylePropertyDefinitionDetails* m_details;
	};

	extern const StylePropertyDefinitionContainer StylePropertyDefinitionList;
}
