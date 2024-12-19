#include <Siv3D/Unicode.hpp>
#include <Siv3D/Char.hpp>
#include "XMLLoader.hpp"
#include "TreeContext.hpp"
#include "../Util/StyleValueHelper.hpp"

#include "NodeComponent/XmlAttributeComponent.hpp"
#include "NodeComponent/TextComponent.hpp"
#include "NodeComponent/UIComponent.hpp"

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

		static void LoadAttributes(FlexBoxNode& node, const tinyxml2::XMLElement& element)
		{
			node.clearProperties();
			for (auto attr = element.FirstAttribute(); attr; attr = attr->Next())
			{
				node.setProperty(Unicode::FromUTF8(attr->Name()), Unicode::FromUTF8(attr->Value()));
			}
		}
	}

	bool XMLLoader::load(std::shared_ptr<FlexBoxNode>& rootRef, const tinyxml2::XMLDocument& document)
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

		m_rootCache.reset();
		m_id2NodeDic.clear();

		return result;
	}

	void XMLLoader::registerStateFactory(const String& tagName, std::unique_ptr<UIState>(*factory)())
	{
		assert(tagName);
		assert(tagName.all([](String::value_type c) { return IsAlnum(c) || c == U'.'; }));
		assert(factory);

		String name = tagName;
		name.lowercase();

		m_stateFactories[name] = factory;
	}

	std::shared_ptr<FlexBoxNode> XMLLoader::loadNode(const tinyxml2::XMLElement& element, bool isRoot)
	{
		std::shared_ptr<FlexBoxNode> node;

		String tagName = Unicode::FromUTF8(element.Name());
		tagName.lowercase();

		auto idOrNull = element.Attribute("id");
		String id = idOrNull ? Unicode::FromUTF8(idOrNull) : U"";

		// キャッシュからnodeへ取得、見つからない場合は新規作成、失敗したらnullptr
		if (auto cachedNode = popCachedNode({
			.root = isRoot,
			.id = id,
			.tagName = tagName,
		}))
		{
			node = cachedNode;
		}
		else
		{
			auto createdNode = createNodeFromTagName(tagName);
			node = createdNode;
		}
		assert(node);

		// 属性の読み込み
		detail::LoadAttributes(*node, element);

		// 型ごとに読み込み処理を分岐
		if (node->isTextNode())
		{
			node->getComponent<Component::TextComponent>()
				.setText(detail::LoadInnerText(element));
		}
		else
		{
			if (node->isUINode())
			{
				node->getComponent<Component::UIComponent>()
					.setTextContent(detail::LoadInnerText(element));
			}

			auto newChildren = loadChildren(element);
			node->setChildren(newChildren);
		}
		
		return node;
	}

	Array<std::shared_ptr<FlexBoxNode>> XMLLoader::loadChildren(const tinyxml2::XMLElement& element)
	{
		Array<std::shared_ptr<FlexBoxNode>> children;

		for (auto childElement = element.FirstChildElement();
			childElement;
			childElement = childElement->NextSiblingElement())
		{
			children.push_back(loadNode(*childElement, false));
		}

		return children;
	}

	std::shared_ptr<FlexBoxNode> XMLLoader::createNodeFromTagName(const StringView tagName)
	{
		if (tagName == U"label")
		{
			auto node = std::make_shared<FlexBoxNode>(FlexBoxNodeOptions{
				.textNode = true,
				.uiNode = false
			});
			node->getComponent<Component::XmlAttributeComponent>()
				.setTagName(tagName);
			node->getComponent<Component::TextComponent>()
				.setText(U"");
			return node;
		}

		// UI Node
		if (auto factory = m_stateFactories.find(tagName);
			factory != m_stateFactories.end())
		{
			auto generator = factory->second;
			auto node = std::make_shared<FlexBoxNode>(FlexBoxNodeOptions{
				.textNode = false,
				.uiNode = true
			});
			node->getComponent<Component::XmlAttributeComponent>()
				.setTagName(tagName);
			node->getComponent<Component::UIComponent>()
				.setState(generator());
			return node;
		}

		auto node = std::make_shared<FlexBoxNode>(FlexBoxNodeOptions{
			.textNode = false,
			.uiNode = false
		});
		node->getComponent<Component::XmlAttributeComponent>()
			.setTagName(tagName);
		return node;

		return nullptr;
	}

	void XMLLoader::cacheNodesById(std::shared_ptr<FlexBoxNode> node)
	{
		if (auto id = node->getComponent<Component::XmlAttributeComponent>().id())
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

	std::shared_ptr<FlexBoxNode> XMLLoader::popCachedNode(_CacheFilters filters)
	{
		std::shared_ptr<FlexBoxNode> node;

		if (auto itr = m_id2NodeDic.find(filters.id);
			itr != m_id2NodeDic.end())
		{
			// IDで検索
			node = std::move(itr->second);
			m_id2NodeDic.erase(itr);
		}
		else if (filters.root && m_rootCache)
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

		auto& component = node->getComponent<Component::XmlAttributeComponent>();

		if (filters.id && component.id() != filters.id)
		{
			return nullptr;
		}

		if (filters.tagName && component.tagName() != filters.tagName)
		{
			return nullptr;
		}

		return node;
	}
}
