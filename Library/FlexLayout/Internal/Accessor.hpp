#pragma once
#include "../Box.hpp"
#include "FlexBoxNode.hpp"
#include "../UIState.hpp"

namespace FlexLayout::Internal
{
	struct Accessor
	{
		static std::shared_ptr<FlexBoxNode> GetNode(const FlexLayout::Box& box)
		{
			return box.m_node;
		}
	};
}
