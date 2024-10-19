#include <gtest/gtest.h>
#include <Siv3D.hpp>
#include "FlexLayout/Internal/FlexBoxImpl.hpp"
#include "FlexLayout/Internal/XMLLoader.hpp"

namespace FlexLayout::Internal
{
	TEST(FlexBoxLayoutTest, PropergateOffset)
	{
		std::shared_ptr<FlexBoxImpl> root;

		tinyxml2::XMLDocument document;
		document.Parse(R"(
			<Layout>
				<Box style="margin: 10px">
					<Box/>
				</Box>
			</Layout>
		)");

		XMLLoader{}.load(root, document);

		FlexBoxImpl::ApplyStyles(*root);
		FlexBoxImpl::CalculateLayout(*root, s3d::none, s3d::none);

		auto child = root->children()[0];

		root->setPropergateOffset(true);
		root->setLayoutOffsetRecursive(s3d::Vec2{ 0, 0 }, true);

		ASSERT_EQ(child->layoutOffset(), (s3d::Vec2{ 10, 10 }));

		root->setPropergateOffset(false);
		root->setLayoutOffsetRecursive(s3d::Vec2{ 0, 0 }, true);

		ASSERT_EQ(child->layoutOffset(), (s3d::Vec2{ 0, 0 }));
	}
}
