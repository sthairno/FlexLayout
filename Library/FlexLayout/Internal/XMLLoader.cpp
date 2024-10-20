﻿#include <Siv3D/Unicode.hpp>
#include "XMLLoader.hpp"
#include "LabelImpl.hpp"
#include "TreeContext.hpp"
#include "../Util/StyleValueHelper.hpp"

namespace FlexLayout::Internal
{
	namespace detail
	{
		static std::string ToLower(const std::string_view& str)
		{
			std::string result{ str };
			std::transform(
				result.cbegin(),
				result.cend(),
				result.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); }
			);
			return result;
		}

		static const tinyxml2::XMLElement* FirstChildElement(const tinyxml2::XMLElement* element, const std::string_view& name)
		{
			for (auto child = element->FirstChildElement(); child; child = child->NextSiblingElement())
			{
				if (ToLower(child->Name()) == name)
				{
					return child;
				}
			}

			return nullptr;
		}

		static String LoadInnerText(const tinyxml2::XMLElement& element)
		{
			std::string innerText;

			for (auto child = element.FirstChild(); child; child = child->NextSibling())
			{
				if (auto childText = child->ToText())
				{
					innerText += childText->Value();
				}
				else if (auto childElement = child->ToElement())
				{
					if (childElement->Name() == std::string_view{ "br" })
					{
						innerText += '\n';
					}
				}
			}

			return Unicode::FromUTF8(innerText);
		}

		static void LoadAttributes(FlexBoxImpl& node, const tinyxml2::XMLElement& element)
		{
			node.clearProperties();
			for (auto attr = element.FirstAttribute(); attr; attr = attr->Next())
			{
				node.setProperty(Unicode::FromUTF8(attr->Name()), Unicode::FromUTF8(attr->Value()));
			}
		}
	}

	bool XMLLoader::load(std::shared_ptr<FlexBoxImpl>& rootRef, const tinyxml2::XMLDocument& document)
	{
		if (document.Error())
		{
			return false;
		}

		auto rootElement = document.RootElement();
		auto rootName = detail::ToLower(rootElement->Name());
		if (!rootElement)
		{
			return false;
		}

		// キャッシュ
		if (rootRef)
		{
			m_rootCache = rootRef;
			cacheNodesById(rootRef);
		}

		// 読み込み
		bool result = false;
		if (rootName == "layout")
		{
			// 独自フォーマットのXMLとして

			if (auto childElement = rootElement->FirstChildElement())
			{
				rootRef = loadNode(*childElement, true);
			}
			else
			{
				rootRef.reset();
			}

			result = true;
		}
		else if (rootName == "html")
		{
			// 簡易的なHTMLとして

			if (auto bodyElement = detail::FirstChildElement(rootElement, "body"))
			{
				rootRef = loadBodyNode(*bodyElement);
			}
			else
			{
				rootRef.reset();
			}

			result = true;
		}

		m_rootCache.reset();
		m_id2NodeDic.clear();

		return result;
	}

	std::shared_ptr<FlexBoxImpl> XMLLoader::loadBodyNode(const tinyxml2::XMLElement& element)
	{
		auto idOrNull = element.Attribute("id");
		String id = idOrNull ? Unicode::FromUTF8(idOrNull) : U"";

		// キャッシュまたは新規ノード
		auto node = popCachedNode({
			.id = id,
			.type = NodeType::Box,
			.tagName = U"body",
		}, true);
		if (!node)
		{
			node = std::make_shared<FlexBoxImpl>(U"body");
		}

		// 属性の読み込み
		detail::LoadAttributes(*node, element);

		// 子要素の読み込み
		auto newChildren = loadChildren(element);
		node->setChildren(newChildren);

		return node;
	}

	std::shared_ptr<FlexBoxImpl> XMLLoader::loadNode(const tinyxml2::XMLElement& element, bool isRoot)
	{
		std::shared_ptr<FlexBoxImpl> node;

		String tagName = Unicode::FromUTF8(element.Name());
		tagName.lowercase();

		auto idOrNull = element.Attribute("id");
		String id = idOrNull ? Unicode::FromUTF8(idOrNull) : U"";

		// キャッシュからnodeへ取得、見つからない場合は新規作成、失敗したらnullptr
		if (auto cachedNode = popCachedNode({
			.id = id,
			.type = isRoot ? MakeOptional(NodeType::Box) : none
		}, isRoot))
		{
			node = cachedNode;
		}
		else if (auto createdNode = createNodeFromTagName(tagName))
		{
			node = createdNode;
		}
		else
		{
			return nullptr;
		}

		// 属性の読み込み
		detail::LoadAttributes(*node, element);

		// 型ごとに読み込み処理を分岐
		switch (node->type())
		{
		case NodeType::Box:
		{
			auto newChildren = loadChildren(element);
			node->setChildren(newChildren);
		}
		break;
		case NodeType::Label:
		{
			reinterpret_cast<LabelImpl&>(*node)
				.setText(detail::LoadInnerText(element));
		}
		break;
		}
		
		return node;
	}

	Array<std::shared_ptr<FlexBoxImpl>> XMLLoader::loadChildren(const tinyxml2::XMLElement& element)
	{
		Array<std::shared_ptr<FlexBoxImpl>> children;

		for (auto childElement = element.FirstChildElement();
			childElement;
			childElement = childElement->NextSiblingElement())
		{
			children.push_back(loadNode(*childElement, false));
		}

		return children;
	}

	std::shared_ptr<FlexBoxImpl> XMLLoader::createNodeFromTagName(const StringView tagName)
	{
		if (tagName == U"label")
		{
			return std::make_shared<LabelImpl>(tagName);
		}

		if (tagName == U"box")
		{
			return std::make_shared<FlexBoxImpl>(tagName);
		}

		return nullptr;
	}

	void XMLLoader::cacheNodesById(std::shared_ptr<FlexBoxImpl> node)
	{
		if (auto id = node->id())
		{
			auto itr = m_id2NodeDic.find(*id);
			if (itr == m_id2NodeDic.end())
			{
				m_id2NodeDic.emplace(*id, node);
			}
		}

		for (auto& child : node->children())
		{
			cacheNodesById(child);
		}
	}

	std::shared_ptr<FlexBoxImpl> XMLLoader::popCachedNode(_CacheFilters filters, bool useRootCache)
	{
		std::shared_ptr<FlexBoxImpl> node;

		if (auto itr = m_id2NodeDic.find(filters.id);
			itr != m_id2NodeDic.end())
		{
			// IDで検索
			node = std::move(itr->second);
			m_id2NodeDic.erase(itr);
		}
		else if (useRootCache && m_rootCache)
		{
			// ルート要素のキャッシュを利用
			node = m_rootCache;
		}
		else
		{
			// ヒットしない場合はnullptr
			return nullptr;
		}

		// フィルターに合致しない場合はnullptr

		if (filters.id && node->id() != filters.id)
		{
			return nullptr;
		}

		if (filters.type && node->type() != *filters.type)
		{
			return nullptr;
		}

		if (filters.tagName && node->tagName() != filters.tagName)
		{
			return nullptr;
		}

		return node;
	}
}
