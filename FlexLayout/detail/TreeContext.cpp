#include "TreeContext.hpp"
#include "FlexBoxImpl.hpp"

namespace FlexLayout::detail
{
	TreeContext::TreeContext()
		: m_yogaConfig{ YGConfigNew() }
		, m_dummyNode{ YGNodeNewWithConfig(m_yogaConfig) }
	{ }

	void TreeContext::loadBegin()
	{ }

	void TreeContext::loadEnd()
	{
		m_id2NodeDic = std::move(m_id2NodeDicNext);
		m_id2NodeDicNext.clear();
	}

	YGNodeRef TreeContext::createNode() const
	{
		return YGNodeNewWithConfig(m_yogaConfig);
	}

	void TreeContext::resetNodeStyle(YGNodeRef node) const
	{
		YGNodeCopyStyle(node, m_dummyNode);
	}

	std::shared_ptr<FlexBoxImpl> TreeContext::popFlexBox(const StringView id)
	{
		auto itr = m_id2NodeDic.find(id);
		if (itr == m_id2NodeDic.end())
		{
			return nullptr;
		}
		else
		{
			auto tmp = std::move(itr->second);
			m_id2NodeDic.erase(itr);
			return tmp;
		}
	}

	bool TreeContext::pushFlexBox(const std::shared_ptr<FlexBoxImpl> item)
	{
		auto id = item->id();
		assert(id);

		auto itr = m_id2NodeDicNext.find(*id);
		if (itr == m_id2NodeDicNext.end())
		{
			m_id2NodeDicNext.emplace(*id, std::move(item));
			return true;
		}

		return false;
	}

	bool TreeContext::useWebDefaults() const
	{
		return YGConfigGetUseWebDefaults(m_yogaConfig);
	}

	void TreeContext::setUseWebDefaults(bool value)
	{
		YGConfigSetUseWebDefaults(m_yogaConfig, value);
	}

	TreeContext::~TreeContext()
	{
		YGNodeFree(m_dummyNode);
		m_dummyNode = nullptr;

		YGConfigFree(m_yogaConfig);
		m_yogaConfig = nullptr;
	}
}
