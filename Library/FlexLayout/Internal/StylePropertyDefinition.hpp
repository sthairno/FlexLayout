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

	struct StylePropertyDefinition
	{
		/// @brief 受け付ける入力値のパターン
		const std::vector<std::vector<Style::detail::StyleValueMultiMatchRule>> patterns;

		/// @brief 入力値をFlexBoxImplに反映させるコールバック
		const StyleInstallCallback installCallback;

		/// @brief プロパティを初期設定に戻すコールバック
		const StyleResetCallback resetCallback;
	};

	extern const HashTable<s3d::StringView, StylePropertyDefinition> StylePropertyDefinitionList;
}
