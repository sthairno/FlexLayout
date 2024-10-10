#include "Config.hpp"
#include <Siv3D/FontAsset.hpp>

namespace FlexLayout::Internal
{
	namespace detail
	{
		static std::unique_ptr<Config> config;
	}

	Config::Config()
		: m_yogaConfig{ YGConfigNew() }
		, m_dummyNode{ nullptr }
		, m_defaultTextStyle{
			.font = SimpleGUI::GetFont(),
			.fontSizePx = 16.0F,
			.lineHeightMul = 1.2F,
			.textAlign = TextAlign::Start
		}
	{
		YGConfigSetUseWebDefaults(m_yogaConfig, true);
		m_dummyNode = YGNodeNewWithConfig(m_yogaConfig);
	}

	YGNodeRef Config::createNode() const
	{
		return YGNodeNewWithConfig(m_yogaConfig);
	}

	void Config::resetNodeStyle(YGNodeRef node) const
	{
		YGNodeCopyStyle(node, m_dummyNode);
	}

	bool Config::useWebDefaults() const
	{
		return YGConfigGetUseWebDefaults(m_yogaConfig);
	}

	void Config::setUseWebDefaults(bool value)
	{
		YGConfigSetUseWebDefaults(m_yogaConfig, value);

		YGNodeFree(m_dummyNode);
		m_dummyNode = YGNodeNewWithConfig(m_yogaConfig);
	}

	void Config::setFontLoader(std::function<Font(StringView)> loader)
	{
		assert(loader);
		m_fontLoader = std::move(loader);
	}

	Font Config::loadFont(StringView id) const
	{
		return m_fontLoader(id);
	}

	Config::~Config()
	{
		YGNodeFree(m_dummyNode);
		m_dummyNode = nullptr;

		YGConfigFree(m_yogaConfig);
		m_yogaConfig = nullptr;
	}

	Font Config::DefaultFontLoader(StringView id)
	{
		FontAsset::Wait(id);
		return FontAsset{ id };
	}

	Config& GetConfig()
	{
		// 遅延読み込み
		if (!detail::config)
		{
			detail::config = std::make_unique<Config>();
		}
		return *detail::config;
	}
}
