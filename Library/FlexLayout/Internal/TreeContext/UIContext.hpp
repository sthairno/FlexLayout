#pragma once

namespace FlexLayout::Internal
{
	class FlexBoxNode;
}

namespace FlexLayout::Internal::Context
{
	class UIContext
	{
	public:

		void update(FlexBoxNode& node);

		void draw(FlexBoxNode& node);
	};
}
