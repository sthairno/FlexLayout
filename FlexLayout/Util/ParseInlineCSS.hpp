#pragma once
#include <Siv3D.hpp>

namespace FlexLayout::Util
{
	Array<std::pair<String, String>> ParseInlineCSS(const String& css);
}
