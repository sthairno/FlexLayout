#include "TreeContext.hpp"
#include "FlexBoxImpl.hpp"

namespace FlexLayout::Internal
{
	TreeContext::TreeContext()
		: m_yogaConfig{ YGConfigNew() }
		, m_dummyNode{ YGNodeNewWithConfig(m_yogaConfig) }
	{ }

	YGNodeRef TreeContext::createNode() const
	{
		return YGNodeNewWithConfig(m_yogaConfig);
	}

	void TreeContext::resetNodeStyle(YGNodeRef node) const
	{
		YGNodeCopyStyle(node, m_dummyNode);
	}

	bool TreeContext::useWebDefaults() const
	{
		return YGConfigGetUseWebDefaults(m_yogaConfig);
	}

	void TreeContext::setUseWebDefaults(bool value)
	{
		YGConfigSetUseWebDefaults(m_yogaConfig, value);

		YGNodeFree(m_dummyNode);
		m_dummyNode = YGNodeNewWithConfig(m_yogaConfig);
	}

	TreeContext::~TreeContext()
	{
		YGNodeFree(m_dummyNode);
		m_dummyNode = nullptr;

		YGConfigFree(m_yogaConfig);
		m_yogaConfig = nullptr;
	}
}
