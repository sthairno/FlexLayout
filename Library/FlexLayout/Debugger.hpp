#pragma once
#include "Box.hpp"

namespace FlexLayout
{
	class Debugger
	{
	public:

		static s3d::String DumpTree(Box& root);
	};
}
