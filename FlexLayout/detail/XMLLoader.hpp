#pragma once
#include "Common.hpp"
#include "FlexBoxImpl.hpp"

namespace FlexLayout::detail
{
	class XMLLoader
	{
	public:

		XMLLoader(std::shared_ptr<TreeContext> context)
			: m_context(context)
		{ }

	public:

		bool load(std::shared_ptr<FlexBoxImpl>& rootRef, const tinyxml2::XMLDocument& document);

	private:

		std::shared_ptr<TreeContext> m_context;

		HashTable<String, std::shared_ptr<FlexBoxImpl>> m_id2NodeDic;

		HashTable<String, std::shared_ptr<FlexBoxImpl>> m_id2NodeDicNext;

		std::shared_ptr<FlexBoxImpl> loadNode(const tinyxml2::XMLElement& element);

		Array<std::shared_ptr<FlexBoxImpl>> loadChildren(const tinyxml2::XMLElement& element);

		String loadInnerText(const tinyxml2::XMLElement& element);

		std::shared_ptr<FlexBoxImpl> createNode(const StringView tagName);

		bool pushNode(std::shared_ptr<FlexBoxImpl> item);

		std::shared_ptr<FlexBoxImpl> popNode(const StringView id);

		void loadBegin();

		void loadEnd();
	};
}
