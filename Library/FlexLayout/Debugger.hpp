#pragma once
#include <Siv3D/Cursor.hpp>
#include "Box.hpp"

namespace FlexLayout
{
	class Debugger
	{
	public:

		/// @brief ツリー構造を文字列として出力
		static s3d::String DumpTree(const Box& root);

		/// @brief カーソルを合わせているノードを取得
		/// @remark propergateOffsetフラグがfalseのノードの子要素は無視されます
		static s3d::Optional<Box> GetHoveredBox(const Box& root, const s3d::Vec2& cursorPos = s3d::Cursor::PosF());

		/// @brief レイアウト領域を描画
		static bool DrawLayout(const Box& box);

		/// @brief カーソルを合わせているノードのレイアウト領域を描画
		/// @remark propergateOffsetフラグがfalseのノードの子要素は無視されます
		static bool DrawHoveredBoxLayout(const Box& root, const s3d::Vec2& cursorPos = s3d::Cursor::PosF());
	};
}
