﻿#pragma once
#include <memory>
#include <Siv3D/Array.hpp>
#include <Siv3D/Font.hpp>
#include "Style/StyleValue.hpp"

namespace FlexLayout::Internal
{
	class FlexBoxNode;
}

namespace FlexLayout
{
	class Box;

	class UIStateQuery
	{
	public:

		UIStateQuery(Internal::FlexBoxNode& node)
			: m_node(node)
		{ }

	public:

		// Style

		/// @brief 指定されたスタイルの値を取得する
		/// @return 値が設定されていない場合は空の配列を返す
		s3d::Array<Style::StyleValue> getStyle(const s3d::StringView styleName) const;

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(const s3d::StringView styleName, const s3d::Array<Style::StyleValue>& list);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(const s3d::StringView styleName, Style::StyleValue value);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(const s3d::StringView styleName, Style::StyleValue v1, Style::StyleValue v2);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(const s3d::StringView styleName, Style::StyleValue v1, Style::StyleValue v2, Style::StyleValue v3);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(const s3d::StringView styleName, Style::StyleValue v1, Style::StyleValue v2, Style::StyleValue v3, Style::StyleValue v4);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(
			const s3d::StringView styleName,
			Style::ValueInputVariant value
		);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(
			const s3d::StringView styleName,
			Style::ValueInputVariant v1,
			Style::ValueInputVariant v2
		);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(
			const s3d::StringView styleName,
			Style::ValueInputVariant v1,
			Style::ValueInputVariant v2,
			Style::ValueInputVariant v3
		);

		/// @brief 指定されたスタイルの値を設定する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool setStyle(
			const s3d::StringView styleName,
			Style::ValueInputVariant v1,
			Style::ValueInputVariant v2,
			Style::ValueInputVariant v3,
			Style::ValueInputVariant v4
		);

		/// @brief 指定されたスタイルの値を削除する
		/// @return 成功した場合はtrue, 失敗した場合はfalse
		bool unsetStyle(const s3d::StringView styleName);

		// Font

		/// @brief フォントの設定値を取得する
		s3d::Font font() const;

		/// @brief 使用するフォントを設定する
		/// @remark 無効なフォントの場合、親要素またはデフォルトのフォント設定を継承します
		void setFont(s3d::Font font);

		/// @brief フォントの設定を解除する
		void unsetFont();

	private:

		Internal::FlexBoxNode& m_node;
	};

	class UIState
	{
	public:

		UIState() { }

	public:

		virtual void attach(UIStateQuery) { };

		virtual void detach(UIStateQuery) { };

		virtual void draw(const Box&) { };

		virtual void update(const Box&) { };

		virtual void setProperty(const s3d::StringView, const s3d::StringView) { };

		virtual void unsetProperty(const s3d::StringView) { };

		virtual std::unique_ptr<UIState> clone() = 0;

	public:

		virtual ~UIState() { };
	};
}
