#pragma once
#include "../Box.hpp"
#include "../Label.hpp"
#include "FlexBoxImpl.hpp"
#include "LabelImpl.hpp"

namespace FlexLayout::Internal
{
	struct BoxAccessor
	{
		static std::shared_ptr<FlexBoxImpl> GetImpl(const FlexLayout::Box& box)
		{
			return box.m_impl;
		}

		static std::shared_ptr<LabelImpl> GetLabelImpl(const FlexLayout::Label& label)
		{
			return std::reinterpret_pointer_cast<LabelImpl>(label.m_impl);
		}
	};
}
