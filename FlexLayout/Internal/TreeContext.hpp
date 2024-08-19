#pragma once
#include "Common.hpp"

namespace FlexLayout::Internal
{
	class FlexBoxImpl;

	class TreeContext
	{
	public:

		TreeContext();

	public:

		YGNodeRef createNode() const;

		void resetNodeStyle(YGNodeRef node) const;

		bool useWebDefaults() const;

		void setUseWebDefaults(bool value);

	private:

		YGConfigRef m_yogaConfig;

		YGNodeRef m_dummyNode;

	public:

		~TreeContext();
	};
}
