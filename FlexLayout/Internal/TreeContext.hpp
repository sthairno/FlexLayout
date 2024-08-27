#pragma once
#include "Common.hpp"
#include "ComputedTextStyle.hpp"

namespace FlexLayout::Internal
{
	class FlexBoxImpl;

	class TreeContext
	{
	public:

		TreeContext();

	public:

		bool useWebDefaults() const;

		void setUseWebDefaults(bool value);

		const ComputedTextStyle& defaultTextStyle() const { return m_defaultTextStyle; }

	private:

		friend FlexBoxImpl;

		using _WaitlistComp = decltype([](const auto& a, const auto& b) { return a.first < b.first; });

		YGConfigRef m_yogaConfig;

		/// @brief スタイルの初期値を取得するためのダミーノード
		YGNodeRef m_dummyNode;

		ComputedTextStyle m_defaultTextStyle;

		/// @brief `FlexBoxImpl::scheduleStyleApplication()`でスケジュールされた要素の待機リスト
		std::set<std::pair<size_t, std::weak_ptr<FlexBoxImpl>>, _WaitlistComp> m_styleApplicationWaitlist;

		YGNodeRef createNode() const;

		void resetNodeStyle(YGNodeRef node) const;

	public:

		~TreeContext();
	};
}
