#pragma once
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

		const std::shared_ptr<TreeContext> m_context;

		std::shared_ptr<FlexBoxImpl> m_rootCache;

		HashTable<String, std::shared_ptr<FlexBoxImpl>> m_id2NodeDic;

		HashTable<String, std::shared_ptr<FlexBoxImpl>> m_id2NodeDicNext;

		std::shared_ptr<FlexBoxImpl> loadNode(const tinyxml2::XMLElement& element, bool isRoot = false);

		Array<std::shared_ptr<FlexBoxImpl>> loadChildren(const tinyxml2::XMLElement& element);

		String loadInnerText(const tinyxml2::XMLElement& element);

		std::shared_ptr<FlexBoxImpl> createNode(const StringView tagName);

		bool pushNode(std::shared_ptr<FlexBoxImpl> item);

		std::shared_ptr<FlexBoxImpl> popNode(const StringView id);
	};
}
