#pragma once
#include "../Box.hpp"
#include "FlexBoxImpl.hpp"

namespace FlexLayout::Internal
{
	struct BoxAccessor
	{
		static std::shared_ptr<FlexBoxImpl> GetImpl(const FlexLayout::Box& box)
		{
			return box.m_impl;
		}
	};
}
