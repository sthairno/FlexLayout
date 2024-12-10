#pragma once
#include <yoga/Yoga.h>
#include <functional>
#include "Style/ComputedTextStyle.hpp"

namespace FlexLayout::Internal
{
	/// @brief アプリケーション共通の設定
	class Config
	{
	public:

		Config();

	public:

		bool useWebDefaults() const;

		void setUseWebDefaults(bool value);

		auto fontLoader() const { return m_fontLoader; }

		void setFontLoader(std::function<Font(StringView)> loader);

		/// @brief 設定を反映したYogaのダミーノード
		YGNodeConstRef dummyNode() const { return m_dummyNode; }

		/// @brief テキスト設定の初期値
		const ComputedTextStyle& defaultTextStyle() const { return m_defaultTextStyle; }

		YGNodeRef createNode() const;

		void resetNodeStyle(YGNodeRef node) const;

		Font loadFont(StringView id) const;

	private:

		YGConfigRef m_yogaConfig;

		/// @brief スタイルの初期値を取得するためのダミーノード
		YGNodeRef m_dummyNode;

		ComputedTextStyle m_defaultTextStyle;

		std::function<Font(StringView)> m_fontLoader = DefaultFontLoader;

		static Font DefaultFontLoader(StringView id);

	public:

		~Config();
	};

	Config& GetConfig();
}
