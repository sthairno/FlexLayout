#pragma once
#include <Siv3D/Array.hpp>

using namespace s3d;

namespace FlexLayout::Internal
{
	class FlexBoxNode;
}

namespace FlexLayout::Internal::Context
{
	class StyleContext
	{
	public:

		void applyProperties(FlexBoxNode& root);
	};
}
