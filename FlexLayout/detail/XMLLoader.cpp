#include "XMLLoader.hpp"
#include "LabelImpl.hpp"

namespace FlexLayout::detail
{
	bool XMLLoader::load(std::shared_ptr<FlexBoxImpl>& rootRef, const tinyxml2::XMLDocument& document)
	{
		if (document.Error())
		{
			return false;
		}

		auto rootElement = document.RootElement();

		if (!rootElement || rootElement->Name() != "Layout"sv)
		{
			return false;
		}

		bool useWebDefaults = false;
		if (rootElement->QueryBoolAttribute("useWebDefaults", &useWebDefaults) == tinyxml2::XML_SUCCESS)
		{
			m_context->setUseWebDefaults(useWebDefaults);
		}

		loadBegin();

		if (auto childElement = rootElement->FirstChildElement())
		{
			rootRef = loadNode(*childElement);
		}
		else
		{
			rootRef.reset();
		}

		loadEnd();

		return true;
	}

	std::shared_ptr<FlexBoxImpl> XMLLoader::loadNode(const tinyxml2::XMLElement& element)
	{
		std::shared_ptr<FlexBoxImpl> node;

		auto idSrc = element.Attribute("id");

		String tagName = Unicode::FromUTF8(element.Name());
		String id = idSrc ? Unicode::FromUTF8(idSrc) : U"";

		if (auto cachedNode = popNode(id); cachedNode && cachedNode->tagName() == tagName)
		{
			node = cachedNode;
		}
		else if (auto createdNode = createNode(tagName))
		{
			node = createdNode;
		}
		else
		{
			return nullptr;
		}

		if (node->id())
		{
			pushNode(node);
		}

		// 属性の読み込み
		node->clearProperties();
		for (auto attr = element.FirstAttribute(); attr; attr = attr->Next())
		{
			node->setProperty(Unicode::FromUTF8(attr->Name()), Unicode::FromUTF8(attr->Value()));
		}

		// タグごとに処理を分岐
		if (node->tagName() == U"Label")
		{
			node->removeChildren();
			reinterpret_cast<LabelImpl&>(*node)
				.setText(loadInnerText(element));
		}
		else if (node->tagName() == U"Box")
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
		if (tagName == U"Label")
		{
			return std::make_shared<FlexBoxImpl>(m_context, tagName);
		}
		else if (tagName == U"Box")
		{
			return std::make_shared<FlexBoxImpl>(m_context, tagName);
		}
		else
		{
			return nullptr;
		}
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

	void XMLLoader::loadBegin()
	{ }

	void XMLLoader::loadEnd()
	{
		m_id2NodeDic = std::move(m_id2NodeDicNext);
		m_id2NodeDicNext.clear();
	}
}
