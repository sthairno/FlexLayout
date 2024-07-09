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

		void loadBegin();

		void loadEnd();

		YGNodeRef createNode() const;

		void resetNodeStyle(YGNodeRef node) const;

		std::shared_ptr<FlexBoxImpl> popFlexBox(const StringView id);

		bool pushFlexBox(const std::shared_ptr<FlexBoxImpl> item);

		bool useWebDefaults() const;

		void setUseWebDefaults(bool value);

	private:

		HashTable<String, std::shared_ptr<FlexBoxImpl>> m_id2NodeDic;

		HashTable<String, std::shared_ptr<FlexBoxImpl>> m_id2NodeDicNext;

		YGConfigRef m_yogaConfig;

		YGNodeRef m_dummyNode;

	public:

		~TreeContext();
	};
}
