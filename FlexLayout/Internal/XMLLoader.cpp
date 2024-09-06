#include "XMLLoader.hpp"
#include "LabelImpl.hpp"
#include "TreeContext.hpp"

namespace FlexLayout::Internal
{
	namespace detail
	{
		std::string ToLower(const std::string_view& str)
		{
			std::string result{ str };
			std::transform(
				result.cbegin(),
				result.cend(),
				result.begin(),
				[](unsigned char c) { return std::tolower(c); }
			);
			return result;
		}
	}

	XMLLoader::XMLLoader(std::shared_ptr<TreeContext> context)
		: m_context(context)
	{
		assert(m_context);
	}

	bool XMLLoader::load(std::shared_ptr<FlexBoxImpl>& rootRef, const tinyxml2::XMLDocument& document)
	{
		if (document.Error())
		{
			return false;
		}

		auto rootElement = document.RootElement();
		if (!rootElement || detail::ToLower(rootElement->Name()) != "layout")
		{
			return false;
		}

		if (auto childElement = rootElement->FirstChildElement())
		{
			rootRef = loadNode(*childElement, true);
		}
		else
		{
			rootRef.reset();
		}

		m_id2NodeDic = std::move(m_id2NodeDicNext);
		m_id2NodeDicNext.clear();

		return true;
	}

	void XMLLoader::clearCache()
	{
		m_id2NodeDic.clear();
		m_id2NodeDicNext.clear();
		m_rootCache.reset();	
	}

	std::shared_ptr<FlexBoxImpl> XMLLoader::loadNode(const tinyxml2::XMLElement& element, bool isRoot)
	{
		std::shared_ptr<FlexBoxImpl> node;

		String tagName = Unicode::FromUTF8(element.Name());
		tagName.lowercase();

		auto idOrNull = element.Attribute("id");
		String id = idOrNull ? Unicode::FromUTF8(idOrNull) : U"";

		// キャッシュからnodeへ取得、見つからない場合は新規作成
		auto cachedNode = popNode(id);
		if (cachedNode && cachedNode->tagName() == tagName)
		{
			node = cachedNode;
		}
		else if (isRoot && m_rootCache)
		{
			node = m_rootCache;
		}
		else if (auto createdNode = createNode(tagName))
		{
			// キャッシュにヒットしない場合は新規作成
			node = createdNode;
		}
		else
		{
			return nullptr;
		}

		// 次回読み込みのためにキャッシュに保存
		if (node->id())
		{
			pushNode(node);
		}
		if (isRoot)
		{
			m_rootCache = node;
		}

		// 属性の読み込み
		node->clearProperties();
		for (auto attr = element.FirstAttribute(); attr; attr = attr->Next())
		{
			node->setProperty(Unicode::FromUTF8(attr->Name()), Unicode::FromUTF8(attr->Value()));
		}

		// タグごとに処理を分岐
		if (node->tagName() == U"label")
		{
			node->removeChildren();
			reinterpret_cast<LabelImpl&>(*node)
				.setText(loadInnerText(element));
		}
		else if (node->tagName() == U"box")
		{
			auto newChildren = loadChildren(element);
			node->setChildren(newChildren);
		}
		else
		{
			// よくわからないタグは無視
			node->removeChildren();
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
			children.push_back(loadNode(*childElement));
		}

		return children;
	}

	String XMLLoader::loadInnerText(const tinyxml2::XMLElement& element)
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
				if (childElement->Name() == "br"sv)
				{
					innerText += '\n';
				}
			}
		}

		return Unicode::FromUTF8(innerText);
	}

	std::shared_ptr<FlexBoxImpl> XMLLoader::createNode(const StringView tagName)
	{
		if (tagName == U"label")
		{
			return std::make_shared<LabelImpl>(m_context, tagName);
		}

		if (tagName == U"box")
		{
			return std::make_shared<FlexBoxImpl>(m_context, tagName);
		}
		
		return nullptr;
	}

	bool XMLLoader::pushNode(std::shared_ptr<FlexBoxImpl> item)
	{
		auto id = item->id();
		assert(id);

		auto itr = m_id2NodeDicNext.find(*id);
		if (itr == m_id2NodeDicNext.end())
		{
			m_id2NodeDicNext.emplace(*id, std::move(item));
			return true;
		}

		return false;
	}

	std::shared_ptr<FlexBoxImpl> XMLLoader::popNode(const StringView id)
	{
		auto itr = m_id2NodeDic.find(id);
		if (itr == m_id2NodeDic.end())
		{
			return nullptr;
		}
		else
		{
			auto tmp = std::move(itr->second);
			m_id2NodeDic.erase(itr);
			return tmp;
		}
	}
}
