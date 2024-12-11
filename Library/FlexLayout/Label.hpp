#pragma once
#include <Siv3D/TextStyle.hpp>
#include "Box.hpp"

namespace FlexLayout
{
	class Label : public Box
	{
	public:

		using Box::Box;

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
	};
}
