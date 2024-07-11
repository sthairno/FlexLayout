#include "FlexBoxImpl.hpp"

namespace FlexLayout::detail
{
	void FlexBoxImpl::load(FlexBoxImpl* parent, const tinyxml2::XMLElement* element)
	{
		assert(element);

		// 属性の読み込み
		clearProperties();
		for (auto attr = element->FirstAttribute(); attr; attr = attr->Next())
		{
			setProperty(Unicode::FromUTF8(attr->Name()), Unicode::FromUTF8(attr->Value()));
		}
		

		// IDの登録
		if (m_id)
		{
			m_context->pushFlexBox(shared_from_this());
		}

		// LabelStatusの更新
		m_labelStatus = LabelStatus::Create(m_labelProperty, parent ? parent->labelStatus() : LabelStatus{});

		// タグごとに分岐
		m_tagName = Unicode::FromUTF8(element->Name());

		if (m_tagName == U"Label")
		{
			removeChildren();
			loadLabel(element);
		}
		else if (m_tagName == U"Box")
		{
			loadChildren(element);
		}
		else
		{
			// よくわからないタグは無視
			removeChildren();
		}
	}

	void FlexBoxImpl::loadChildren(const tinyxml2::XMLElement* element)
	{
		Array<std::shared_ptr<FlexBoxImpl>> newChildren;

		for (auto childElement = element->FirstChildElement();
			childElement;
			childElement = childElement->NextSiblingElement())
		{
			auto childId = childElement->Attribute("id");

			if (childId)
			{
				if (auto child = m_context->popFlexBox(Unicode::FromUTF8(childId)))
				{
					child->load(this, childElement);
					newChildren.push_back(child);
					continue;
				}
			}

			auto child = std::make_shared<FlexBoxImpl>(m_context);
			child->load(this, childElement);
			newChildren.push_back(child);
		}

		setChildren(newChildren);
	}

	void FlexBoxImpl::loadLabel(const tinyxml2::XMLElement* element)
	{
		std::string innerText;
		for (auto child = element->FirstChild(); child; child = child->NextSibling())
		{
			if (auto childText = child->ToText())
			{
				innerText += childText->Value();
			}
			if (auto childElement = child->ToElement())
			{
				if (childElement->Name() == "br"sv)
				{
					innerText += '\n';
				}
			}
		}

		m_labelRenderer = std::make_unique<LabelRenderer>(m_labelStatus, Unicode::FromUTF8(innerText));

		const static YGMeasureFunc measureFunc = [](
			YGNodeConstRef node,
			float width,
			YGMeasureMode widthMode,
			float height,
			YGMeasureMode heightMode) -> YGSize
		{
			auto& impl = *reinterpret_cast<FlexBoxImpl*>(YGNodeGetContext(node));
			auto renderer = impl.labelRenderer();

			assert(renderer);

			renderer->build(widthMode != YGMeasureModeUndefined ? width : Inf<double>);
			auto measuredSize = renderer->size();

			YGSize result{
				static_cast<float>(measuredSize.x),
				static_cast<float>(measuredSize.y)
			};

			switch (widthMode)
			{
			case YGMeasureModeUndefined:
				break;
			case YGMeasureModeAtMost:
				result.width = Min(result.width, width);
				break;
			case YGMeasureModeExactly:
				result.width = width;
				break;
			}

			switch (heightMode)
			{
			case YGMeasureModeUndefined:
				break;
			case YGMeasureModeAtMost:
				result.height = Min(result.height, height);
				break;
			case YGMeasureModeExactly:
				result.height = height;
				break;
			}

			return result;
		};
		const static YGBaselineFunc baselineFunc = [](YGNodeConstRef node, float, float) -> float
		{
			auto& impl = *reinterpret_cast<FlexBoxImpl*>(YGNodeGetContext(node));
			auto renderer = impl.labelRenderer();

			assert(renderer);

			return static_cast<float>(renderer->baseline());
		};

		YGNodeSetMeasureFunc(m_node, measureFunc);
		YGNodeSetBaselineFunc(m_node, baselineFunc);
	}
}
