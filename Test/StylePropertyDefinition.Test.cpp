#include <gtest/gtest.h>
#include <Siv3D.hpp>
#include "FlexLayout/Internal/Style/StylePropertyDefinition.hpp"
#include "FlexLayout/Internal/Style/StyleProperty.hpp"

namespace FlexLayout::Internal
{
	// プロパティ名のハッシュ値が一意である
	TEST(StylePropertyDefinitionTest, HashIsUnique)
	{
		std::set<size_t> hashList;
		for (const auto& def : StylePropertyDefinitionList)
		{
			auto [itr, success] = hashList.insert(StyleProperty::Hash(def.first));

			ASSERT_TRUE(success);
		}
	}

	// 定義のmaybeAffectToで指定されたプロパティが存在する
	TEST(StylePropertyDefinitionTest, MaybeAffectToExists)
	{
		for (const auto& def : StylePropertyDefinitionList)
		{
			for (const auto& prop : def.second.maybeAffectTo)
			{
				ASSERT_TRUE(StylePropertyDefinitionList.find(prop) != StylePropertyDefinitionList.end());
			}
		}
	}

	// 定義のmaybeAffectToで指定されたプロパティのmaybeAffectToが空であるか
	// (プロパティが複雑な依存関係を持っていないか)
	TEST(StylePropertyDefinitionTest, MaybeAffectToIsLeaf)
	{
		using namespace FlexLayout::Internal;

		for (const auto& def : StylePropertyDefinitionList)
		{
			for (const auto& prop : def.second.maybeAffectTo)
			{
				ASSERT_TRUE(StylePropertyDefinitionList.at(prop).maybeAffectTo.empty());
			}
		}
	}
}
