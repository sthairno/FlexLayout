#include "Debugger.hpp"
#include "Internal/Accessor.hpp"
#include <Siv3D/Window.hpp>
#include <Siv3D/WindowState.hpp>
#include <vector>
#include "Internal/NodeComponent/XmlAttributeComponent.hpp"
#include "Internal/NodeComponent/LayoutComponent.hpp"

using namespace s3d;

namespace FlexLayout
{
	static constexpr double InspectorColorAlpha = 0.7;
	static constexpr ColorF InspectorMarginColor = ColorF{ Palette::Darkorange, InspectorColorAlpha };
	static constexpr ColorF InspectorBorderColor = ColorF{ Palette::Gold, InspectorColorAlpha };
	static constexpr ColorF InspectorPaddingColor = ColorF{ Palette::Yellowgreen, InspectorColorAlpha };
	static constexpr ColorF InspectorContentColor = ColorF{ Palette::Skyblue, InspectorColorAlpha };

	static void DumpTreeRecursive(const std::shared_ptr<Internal::FlexBoxNode>& node, String& output, std::vector<std::int8_t>& indentPattern)
	{
		// インデント

		for (auto type : indentPattern)
		{
			switch (type)
			{
			case 0:
				output += U"├─ ";
				break;
			case 1:
				output += U"└─ ";
				break;
			case 2:
				output += U"│  ";
				break;
			case 3:
				output += U"   ";
				break;
			}
		}

		// ノード情報

		const auto& xmlComponent = node->getComponent<Internal::Component::XmlAttributeComponent>();

		output += xmlComponent.tagName();
		if (xmlComponent.id() || xmlComponent.classes())
		{
			output += U" [";
			if (xmlComponent.id())
			{
				output += U"id: " + xmlComponent.id().value();
			}
			if (xmlComponent.classes())
			{
				if (xmlComponent.id())
				{
					output += U", ";
				}
				output += U"class: " + xmlComponent.getClassText();
			}
			output += U"]";
		}

		output += U"\n";

		// 子要素

		const auto children = node->children();

		if (children.empty())
		{
			return;
		}

		auto lastChildItr = children.end() - 1;

		if (not indentPattern.empty())
		{
			indentPattern.back() = indentPattern.back() == 1 ? 3 : 2;
		}

		indentPattern.push_back(0);
		for (auto itr = children.begin(); itr != children.end(); itr++)
		{
			indentPattern.back() = (itr == lastChildItr) ? 1 : 0;
			DumpTreeRecursive(*itr, output, indentPattern);
		}
		indentPattern.pop_back();
	}

	static std::shared_ptr<Internal::FlexBoxNode> GetHoveredNode(const std::shared_ptr<Internal::FlexBoxNode>& node, const Vec2& cursorPos)
	{
		auto& layout = node->getComponent<Internal::Component::LayoutComponent>();

		if (not layout.layoutOffset())
		{
			return nullptr;
		}

		std::shared_ptr<Internal::FlexBoxNode> result;

		if (layout.contentAreaRect()->contains(cursorPos))
		{
			result = node;
		}

		if (layout.propergateOffset())
		{
			for (const auto& child : node->children())
			{
				if (const auto hoveredNode = GetHoveredNode(child, cursorPos))
				{
					result = hoveredNode;
				}
			}
		}

		return result;
	}

	String Debugger::DumpTree(const Box& root)
	{
		const auto rootImpl = Internal::Accessor::GetNode(root);

		String output;

		std::vector<std::int8_t> indentPattern;
		DumpTreeRecursive(rootImpl, output, indentPattern);

		output.pop_back(); // 最後の行の改行を削除

		return output;
	}

	s3d::Optional<Box> Debugger::GetHoveredBox(const Box& root, const s3d::Vec2& cursorPos)
	{
		const auto rootImpl = Internal::Accessor::GetNode(root);
		auto& windowStat = Window::GetState();

		if (windowStat.sizeMove || not windowStat.focused)
		{
			return none;
		}

		if (const auto hoveredNode = GetHoveredNode(rootImpl, cursorPos))
		{
			return Box{ hoveredNode };
		}

		return none;
	}

	bool Debugger::DrawLayout(const Box& box)
	{
		const auto rootImpl = Internal::Accessor::GetNode(box);
		auto& layout = rootImpl->getComponent<Internal::Component::LayoutComponent>();

		if (not layout.layoutOffset())
		{
			return false;
		}

		auto marginAreaRect = *layout.marginAreaRect();
		layout.margin().drawPadding(marginAreaRect, InspectorMarginColor);

		auto borderAreaRect = *layout.borderAreaRect();
		layout.border().drawPadding(borderAreaRect, InspectorBorderColor);

		auto paddingAreaRect = *layout.paddingAreaRect();
		layout.padding().drawPadding(paddingAreaRect, InspectorPaddingColor);

		auto contentAreaRect = *layout.contentAreaRect();
		contentAreaRect.draw(InspectorContentColor);

		return true;
	}

	bool Debugger::DrawHoveredBoxLayout(const Box& root, const s3d::Vec2& cursorPos)
	{
		if (const auto hoveredBox = GetHoveredBox(root, cursorPos))
		{
			return DrawLayout(*hoveredBox);
		}
		return true;
	}
}
