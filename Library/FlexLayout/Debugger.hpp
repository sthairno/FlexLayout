#pragma once
#include <Siv3D/Cursor.hpp>
#include "Box.hpp"

namespace FlexLayout
{
	class Debugger
	{
	public:

		static s3d::String DumpTree(const Box& root);

		static s3d::Optional<Box> GetHoveredBox(const Box& root, const s3d::Vec2& cursorPos = s3d::Cursor::PosF());

		static bool DrawLayout(const Box& box);

		static bool DrawHoveredBoxLayout(const Box& root, const s3d::Vec2& cursorPos = s3d::Cursor::PosF());
	};
}
