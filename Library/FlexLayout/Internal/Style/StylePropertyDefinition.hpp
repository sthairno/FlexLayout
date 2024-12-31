#pragma once
#include <functional>
#include <vector>
#include <Siv3D/StringView.hpp>
#include <Siv3D/HeterogeneousLookupHelper.hpp>
#include <Siv3D/HashTable.hpp>
#include "StyleValueMatchRule.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	class FlexBoxNode;

	using StyleInstallCallback = std::function<bool(FlexBoxNode&, std::span<const Style::StyleValue>)>;
	using StyleResetCallback = std::function<void(FlexBoxNode&)>;

	struct StylePropertyDefinition
	{
		/// @brief 受け付ける入力値のパターン
		const std::vector<std::vector<StyleValueMultiMatchRule>> patterns;

		/// @brief 入力値をFlexBoxNodeに反映させるコールバック
		const StyleInstallCallback installCallback;

		/// @brief プロパティを初期設定に戻すコールバック
		const StyleResetCallback resetCallback;

		/// @brief installCallback/resetCallback呼び出し時に影響を与える(可能性のある)プロパティ名
		const std::vector<StringView> maybeAffectTo;

		/// @brief 継承プロパティか
		/// @defails trueの場合、子要素でもinstallCallback/resetCallbackが呼び出されます
		/// @see https://developer.mozilla.org/en-US/docs/Web/CSS/Inheritance
		const bool inherit = false;
	};

	using StylePropertyDefinitionContainer = HashTable<s3d::StringView, StylePropertyDefinition>;

	class StylePropertyDefinitionRef
	{
	public:

		StylePropertyDefinitionRef(StylePropertyDefinitionContainer::const_iterator itr)
			: m_name(itr->first)
			, m_data(&itr->second)
		{
		}

		StylePropertyDefinitionRef(StylePropertyDefinitionContainer::const_pointer ptr)
			: m_name(ptr->first)
			, m_data(&ptr->second)
		{
		}

		const StringView name() const { return m_name; }

		const auto& patterns() const { return m_data->patterns; }

		bool installCallback(FlexBoxNode& impl, std::span<const Style::StyleValue> values) const
		{
			return m_data->installCallback(impl, values);
		}

		void resetCallback(FlexBoxNode& impl) const { return m_data->resetCallback(impl); }

		const auto& maybeAffectTo() const { return m_data->maybeAffectTo; }

		bool inherit() const { return m_data->inherit; }

	private:

		StringView m_name;

		const StylePropertyDefinition* m_data;
	};

	extern const StylePropertyDefinitionContainer StylePropertyDefinitionList;
}
