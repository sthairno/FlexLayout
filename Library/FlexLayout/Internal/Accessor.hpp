#pragma once
#include "../Box.hpp"
#include "../Label.hpp"
#include "FlexBoxNode.hpp"

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
