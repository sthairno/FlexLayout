#pragma once
#include <yoga/Yoga.h>
#include <Siv3D/Font.hpp>
#include <functional>
#include "ComputedTextStyle.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	class FlexBoxImpl;

	/// @brief FlexBoxImplのツリー内で共有されるデータ
	class TreeContext
	{
	public:

		TreeContext();

	public:

		bool useWebDefaults() const;

		void setUseWebDefaults(bool value);

		auto fontLoader() const { return m_fontLoader; }

		void setFontLoader(std::function<Font(StringView)> loader);

		YGNodeConstRef dummyNode() const { return m_dummyNode; }

		const ComputedTextStyle& defaultTextStyle() const { return m_defaultTextStyle; }

	private:

		friend FlexBoxImpl;

		YGConfigRef m_yogaConfig;

		/// @brief スタイルの初期値を取得するためのダミーノード
		YGNodeRef m_dummyNode;

		ComputedTextStyle m_defaultTextStyle;

		/// @brief `FlexBoxImpl::scheduleStyleApplication()`でスケジュールされた要素の待機リスト
		Array<std::weak_ptr<FlexBoxImpl>> m_styleApplicationWaitlist;

		std::function<Font(StringView)> m_fontLoader = DefaultFontLoader;

		YGNodeRef createNode() const;

		void resetNodeStyle(YGNodeRef node) const;

		Font loadFont(StringView id) const;

		static Font DefaultFontLoader(StringView id);

	public:

		~TreeContext();
	};
}
