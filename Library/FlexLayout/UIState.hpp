#pragma once
#include <memory>

namespace FlexLayout::Internal
{
	class FlexBoxNode;
}

namespace FlexLayout
{
	class Box;

	class UIState
	{
	public:

		UIState() { }

	public:

		virtual void attach() { };

		virtual void detach() { };

		virtual void draw(Box&) const { };

		virtual void update(Box&) { };

		virtual std::unique_ptr<UIState> clone() = 0;

	public:

		virtual ~UIState() { };
	};
}
