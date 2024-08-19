#pragma once
#include "../Common.hpp"

namespace FlexLayout::Util
{
	Array<std::pair<String, String>> ParseInlineCSS(const String& css);

	String DumpInlineCSS(const Array<std::pair<String, String>>& list);
}
