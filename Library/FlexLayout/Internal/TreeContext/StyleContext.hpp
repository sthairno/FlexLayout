#pragma once
#include <Siv3D/Array.hpp>

using namespace s3d;

namespace FlexLayout::Internal
{
	class FlexBoxNode;
}

namespace FlexLayout::Internal::Context
{
	class StyleContext
	{
	public:

		void applyStyles(FlexBoxNode& root);

		const auto& styleApplicationWaitinglist() const
		{
			return m_styleApplicationWaitinglist;
		}

		void queueStyleApplication(const std::shared_ptr<FlexBoxNode>& node);

	private:

		Array<std::weak_ptr<FlexBoxNode>> m_styleApplicationWaitinglist;
	};
}
