#pragma once
#include <Siv3D/Array.hpp>

using namespace s3d;

namespace FlexLayout::Internal
{
	class FlexBoxImpl;

	/// @brief FlexBoxImplの同一ツリー内で共有されるデータ
	struct TreeContext
	{
		/// @brief `FlexBoxImpl::scheduleStyleApplication()`でスケジュールされた要素の待機リスト
		s3d::Array<std::weak_ptr<FlexBoxImpl>> m_styleApplicationWaitlist;
	};
}
