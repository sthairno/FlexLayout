#pragma once
#include "Common.hpp"
#include "../Style/StyleValueMatchRule.hpp"

namespace FlexLayout::Internal
{
	class FlexBoxImpl;

	using StyleInstallCallback = std::function<bool(FlexBoxImpl&, std::span<const Style::StyleValue>)>;
	using StyleResetCallback = std::function<void(FlexBoxImpl&)>;

	struct StylePropertyDefinition
	{
		/// @brief 受け付ける入力値のパターン
		std::vector<std::vector<Style::detail::StyleValueMultiMatchRule>> patterns;

		/// @brief 入力値をFlexBoxImplに反映させるコールバック
		StyleInstallCallback installCallback;

		/// @brief プロパティを初期設定に戻すコールバック
		StyleResetCallback resetCallback;
	};

	extern const std::unordered_map<StringView, StylePropertyDefinition, s3d::StringHash> StyleProperties;
}
