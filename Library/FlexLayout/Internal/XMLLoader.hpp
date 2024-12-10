#pragma once
#include <tinyxml2.h>
#include "FlexBoxNode.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	class XMLLoader
	{
	public:

		bool load(std::shared_ptr<FlexBoxNode>& rootRef, const tinyxml2::XMLDocument& document);

	private:

		struct _CacheFilters
		{
			bool root = false;
			const StringView id = U"";
			const StringView tagName = U"";
		};

		std::shared_ptr<FlexBoxNode> m_rootCache;

		HashTable<String, std::shared_ptr<FlexBoxNode>> m_id2NodeDic;

		std::shared_ptr<FlexBoxNode> loadNode(const tinyxml2::XMLElement& element, bool isRoot);

		Array<std::shared_ptr<FlexBoxNode>> loadChildren(const tinyxml2::XMLElement& element);

		std::shared_ptr<FlexBoxNode> createNodeFromTagName(const StringView tagName);

		void cacheNodesById(std::shared_ptr<FlexBoxNode> node);

		std::shared_ptr<FlexBoxNode> popCachedNode(_CacheFilters filters);
	};
}
