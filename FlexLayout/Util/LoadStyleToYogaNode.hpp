#pragma once
#include "../Common.hpp"
#include <yoga/Yoga.h>

namespace FlexLayout::Util
{
	bool LoadStyleToYogaNode(YGNodeRef node, const StringView key, const StringView value);
}
