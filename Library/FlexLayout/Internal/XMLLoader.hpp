﻿#pragma once
#include "Common.hpp"
#include "FlexBoxImpl.hpp"

namespace FlexLayout::Internal
{
	class XMLLoader
	{
	public:

		XMLLoader(std::shared_ptr<TreeContext> context);

	public:

		bool load(std::shared_ptr<FlexBoxImpl>& rootRef, const tinyxml2::XMLDocument& document);

		void clearCache();

	private:

		struct _CacheFilters
		{
			const StringView id = U"";
			Optional<NodeType> type = unspecified;
			const StringView tagName = U"";
		};

		const std::shared_ptr<TreeContext> m_context;

		std::shared_ptr<FlexBoxImpl> m_rootCache;

		HashTable<String, std::shared_ptr<FlexBoxImpl>> m_id2NodeDic;

		HashTable<String, std::shared_ptr<FlexBoxImpl>> m_id2NodeDicNext;

		std::shared_ptr<FlexBoxImpl> loadBodyNode(const tinyxml2::XMLElement& element);

		std::shared_ptr<FlexBoxImpl> loadNode(const tinyxml2::XMLElement& element, bool isRoot);

		Array<std::shared_ptr<FlexBoxImpl>> loadChildren(const tinyxml2::XMLElement& element);

		std::shared_ptr<FlexBoxImpl> createNodeFromTagName(const StringView tagName);

		bool cacheNodeById(std::shared_ptr<FlexBoxImpl> node);

		std::shared_ptr<FlexBoxImpl> getCachedNode(_CacheFilters filters, bool useRootCache = true);
	};
}