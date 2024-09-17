#include <Siv3D/FontAsset.hpp>
#include "TreeContext.hpp"
#include "FlexBoxImpl.hpp"

namespace FlexLayout::Internal
{
	TreeContext::TreeContext()
		: m_yogaConfig{ YGConfigNew() }
		, m_dummyNode{ nullptr }
	{
		YGConfigSetUseWebDefaults(m_yogaConfig, true);
		m_dummyNode = YGNodeNewWithConfig(m_yogaConfig);
	}

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

	void TreeContext::setFontLoader(std::function<Font(StringView)> loader)
	{
		assert(loader);
		m_fontLoader = std::move(loader);
	}

	Font TreeContext::loadFont(StringView id) const
	{
		return m_fontLoader(id);
	}

	TreeContext::~TreeContext()
	{
		YGNodeFree(m_dummyNode);
		m_dummyNode = nullptr;

		YGConfigFree(m_yogaConfig);
		m_yogaConfig = nullptr;
	}

	Font TreeContext::DefaultFontLoader(StringView id)
	{
		FontAsset::Wait(id);
		return FontAsset{ id };
	}
}
