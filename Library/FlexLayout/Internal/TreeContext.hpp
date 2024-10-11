#pragma once
#include <Siv3D/Array.hpp>

using namespace s3d;

namespace FlexLayout::Internal
{
	class FlexBoxImpl;

	/// @brief FlexBoxImplの同一ツリー内で共有されるデータ
	class TreeContext
	{
	public:

		/// @brief `FlexBoxImpl::scheduleStyleApplication()`でスケジュールされた要素の待機リスト
		const auto& styleApplicationWaitinglist() const { return m_styleApplicationWaitinglist; }

		void queueStyleApplication(std::shared_ptr<FlexBoxImpl> node);

		void clearStyleApplicationWaitinglist() { m_styleApplicationWaitinglist.clear(); }

		void onNewNodeJoined(std::shared_ptr<FlexBoxImpl> node);

	private:

		Array<std::weak_ptr<FlexBoxImpl>> m_styleApplicationWaitinglist;
	};
}
