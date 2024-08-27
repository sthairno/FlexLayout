#include "FlexBoxImpl.hpp"

namespace FlexLayout::Internal
{
	void FlexBoxImpl::lookupNodesByClassName(Array<std::shared_ptr<FlexBoxImpl>>& list, const String& className, size_t limit)
	{
		if (list.size() >= limit)
		{
			return;
		}

		if (m_classes.contains(className))
		{
			list.push_back(shared_from_this());
		}

		for (const auto& child : m_children)
		{
			child->lookupNodesByClassName(list, className, limit);
		}
	}

	std::shared_ptr<FlexBoxImpl> FlexBoxImpl::lookupNodeById(const StringView id)
	{
		if (m_id == id)
		{
			return shared_from_this();
		}

		for (const auto& child : m_children)
		{
			if (auto node = child->lookupNodeById(id))
			{
				return node;
			}
		}

		return { };
	}
}
