﻿#pragma once
#include <Siv3D/TextStyle.hpp>
#include "Box.hpp"

namespace FlexLayout
{
	namespace Internal
	{
		class LabelImpl;
	}

	class Label : public Box
	{
	public:

		using Box::Box;

		/// @brief 空のLabelを作成する
		/// @remark appendChild()などでツリーに追加することでレイアウトに反映されます
		/// @param text ラベルで表示する文字列
		/// @return 作成したノード
		[[nodiscard]]
		static Label Create(const s3d::StringView text);

	public:

		/// @brief 描画する文字列を取得する
		/// @remark レイアウトを更新するまでは描画に反映されません
		s3d::String text() const;

		/// @brief 描画する文字列を設定する
		void setText(const s3d::StringView text);

		/// @brief 文字列を描画する
		/// @remark 色付きフォントの場合、colorの設定は無視されます
		void draw(const s3d::ColorF& color = s3d::Palette::White) const;

		/// @brief 文字列を描画する
		/// @remark 色付きフォントの場合、colorの設定は無視されます
		void draw(const s3d::TextStyle& textStyle, const s3d::ColorF& color = s3d::Palette::White) const;

	private:

		Internal::LabelImpl& impl() const;
	};
}
