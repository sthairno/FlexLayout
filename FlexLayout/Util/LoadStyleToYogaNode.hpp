#pragma once
#include <Siv3D.hpp>
#include <yoga/Yoga.h>

namespace FlexLayout::Util
{
	bool LoadStyleToYogaNode(YGNodeRef node, const StringView key, const StringView value);
}
