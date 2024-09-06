#pragma once
#include "Common.hpp"
#include "ComputedTextStyle.hpp"

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

		YGNodeRef createNode() const;

		void resetNodeStyle(YGNodeRef node) const;

	public:

		~TreeContext();
	};
}
