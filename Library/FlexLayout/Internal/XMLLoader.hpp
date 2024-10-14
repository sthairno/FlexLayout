#pragma once
#include <tinyxml2.h>
#include "FlexBoxImpl.hpp"

using namespace s3d;

namespace FlexLayout::Internal
{
	class XMLLoader
	{
	public:

		bool load(std::shared_ptr<FlexBoxImpl>& rootRef, const tinyxml2::XMLDocument& document);

	private:

		struct _CacheFilters
		{
			const StringView id = U"";
			Optional<NodeType> type = unspecified;
			const StringView tagName = U"";
		};

		std::shared_ptr<FlexBoxImpl> m_rootCache;

		HashTable<String, std::shared_ptr<FlexBoxImpl>> m_id2NodeDic;

		std::shared_ptr<FlexBoxImpl> loadBodyNode(const tinyxml2::XMLElement& element);

		std::shared_ptr<FlexBoxImpl> loadNode(const tinyxml2::XMLElement& element, bool isRoot);

		Array<std::shared_ptr<FlexBoxImpl>> loadChildren(const tinyxml2::XMLElement& element);

		std::shared_ptr<FlexBoxImpl> createNodeFromTagName(const StringView tagName);

		void cacheNodesById(std::shared_ptr<FlexBoxImpl> node);

		std::shared_ptr<FlexBoxImpl> popCachedNode(_CacheFilters filters, bool useRootCache = true);
	};
}
