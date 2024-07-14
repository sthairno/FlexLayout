#pragma once
#include "Common.hpp"
#include <yoga/Yoga.h>

namespace FlexLayout::detail
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
