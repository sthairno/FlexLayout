#include <gtest/gtest.h>
#include <Siv3D.hpp>
#include "FlexLayout/Layout.hpp"

namespace FlexLayout
{
	TEST(LayoutTest, CallbackWillCalledAtLeastOnce)
	{
		bool called = false;
		auto callback = [&](FlexLayout::Layout&, FlexLayout::Box&) -> void
			{
				called = true;
			};
		FlexLayout::Layout layout(callback);

		ASSERT_FALSE(called);

		ASSERT_TRUE(layout.load(s3d::Arg::code = U"<Layout></Layout>"));

		ASSERT_TRUE(called);
	}
}
