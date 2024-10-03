#include "Debugger.hpp"
#include "Internal/BoxAccessor.hpp"
#include <vector>

using namespace s3d;

namespace FlexLayout
{
	static void DumpTreeRecursive(const std::shared_ptr<Internal::FlexBoxImpl>& node, String& output, std::vector<std::int8_t>& indentPattern)
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

		const auto tagName = node->tagName();
		const auto id = node->id();
		const auto classes = node->classes();

		output += tagName;
		if (id || classes)
		{
			output += U" [";
			if (id)
			{
				output += U"id: " + id.value();
			}
			if (classes)
			{
				if (id)
				{
					output += U", ";
				}
				output += U"class: " + classes.join(U" ", U"", U"");
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

	String Debugger::DumpTree(Box& root)
	{
		const auto rootImpl = Internal::BoxAccessor::GetImpl(root);

		String output;

		std::vector<std::int8_t> indentPattern;
		DumpTreeRecursive(rootImpl, output, indentPattern);

		output.pop_back(); // 最後の行の改行を削除

		return output;
	}
}
