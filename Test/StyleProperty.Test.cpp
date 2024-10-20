#include <gtest/gtest.h>
#include "FlexLayout/Internal/StyleProperty.hpp"

namespace FlexLayout::Internal
{
	TEST(StylePropertyTableTest, Get_CreateEntryWithRemovedState)
	{
		StylePropertyTable tbl;

		ASSERT_EQ(tbl.group(StylePropertyGroup::Inline).size(), 0);

		auto prop = tbl.get(StylePropertyGroup::Inline, U"top");

		ASSERT_TRUE(prop);
		ASSERT_TRUE(prop->removed());
		ASSERT_EQ(tbl.group(StylePropertyGroup::Inline).size(), 1);
	}

	TEST(StylePropertyTableTest, Get_ReturnNullWhenRequestedInvalidPropertyName)
	{
		StylePropertyTable tbl;

		auto prop = tbl.get(StylePropertyGroup::Inline, U"invalid");

		ASSERT_FALSE(prop);
	}

	TEST(StylePropertyTableTest, Find_SameGroup)
	{
		StylePropertyTable tbl;

		tbl.get(StylePropertyGroup::Inline, U"top"); // create removed property

		ASSERT_TRUE(tbl.find(StylePropertyGroup::Inline, U"top"));
	}

	TEST(StylePropertyTableTest, Find_OtherGroup)
	{
		StylePropertyTable tbl;

		tbl.get(StylePropertyGroup::Inline, U"top"); // create removed property

		ASSERT_FALSE(tbl.find(StylePropertyGroup::StyleSheet, U"top"));
	}

	TEST(StylePropertyTableTest, Find_ShouldReturnPropertyWithHigherPriority)
	{
		StylePropertyTable tbl;

		auto a = tbl.get(StylePropertyGroup::StyleSheet, U"top");
		auto b = tbl.get(StylePropertyGroup::Inline, U"top");

		ASSERT_EQ(tbl.find(U"top"), b);
	}

	TEST(StylePropertyTableTest, KeyHash_ShouldBeEqualToStyleDefinition)
	{
		StylePropertyTable tbl;

		tbl.get(StylePropertyGroup::Inline, U"top", true);
		tbl.get(StylePropertyGroup::Inline, U"bottom", true);
		tbl.get(StylePropertyGroup::Inline, U"left", true);
		tbl.get(StylePropertyGroup::Inline, U"top", true);

		ASSERT_EQ(tbl.group(StylePropertyGroup::Inline)[0].keyHash(), StylePropertyDefinitionList.hash(U"bottom"));
		ASSERT_EQ(tbl.group(StylePropertyGroup::Inline)[1].keyHash(), StylePropertyDefinitionList.hash(U"left"));
		ASSERT_EQ(tbl.group(StylePropertyGroup::Inline)[2].keyHash(), StylePropertyDefinitionList.hash(U"top"));
	}

	TEST(StylePropertyTableTest, Find_NotDefinedPropertyShouldBeNull)
	{
		StylePropertyTable tbl;

		tbl.get(StylePropertyGroup::Inline, U"top");
		tbl.get(StylePropertyGroup::Inline, U"bottom");
		tbl.get(StylePropertyGroup::Inline, U"left");

		ASSERT_TRUE(tbl.find(StylePropertyGroup::Inline, U"top"));
		ASSERT_FALSE(tbl.find(StylePropertyGroup::Inline, U"right"));
	}
}
