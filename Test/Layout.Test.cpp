#include <gtest/gtest.h>
#include <Siv3D.hpp>
#include "FlexLayout/Layout.hpp"

namespace FlexLayout
{
	TEST(LayoutTest, OnLoadCallbackWillNotCalledIfLoadingSuccess)
	{
		bool called = false;
		auto callback = [&](FlexLayout::Layout&, FlexLayout::Box&) -> void
			{
				called = true;
			};
		FlexLayout::Layout layout(callback);

		ASSERT_FALSE(called);

		ASSERT_TRUE(layout.load(s3d::Arg::code = U"<Layout><Box/></Layout>"));

		ASSERT_TRUE(called);
	}

	TEST(LayoutTest, OnLoadCallbackWillNotCalledIfLoadingFails1)
	{
		bool called = false;
		auto callback = [&](FlexLayout::Layout&, FlexLayout::Box&) -> void
			{
				called = true;
			};
		FlexLayout::Layout layout(callback);

		ASSERT_FALSE(called);

		ASSERT_FALSE(layout.load(s3d::Arg::code = U"<Layout></Layout>"));

		ASSERT_FALSE(called);
	}

	TEST(LayoutTest, OnLoadCallbackWillNotCalledIfLoadingFails2)
	{
		bool called = false;
		auto callback = [&](FlexLayout::Layout&, FlexLayout::Box&) -> void
			{
				called = true;
			};
		FlexLayout::Layout layout(callback);

		ASSERT_FALSE(called);

		ASSERT_FALSE(layout.load(s3d::Arg::code = U""));

		ASSERT_FALSE(called);
	}
}
