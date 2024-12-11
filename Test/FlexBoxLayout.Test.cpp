#include <gtest/gtest.h>
#include <Siv3D.hpp>
#include "FlexLayout/Internal/FlexBoxNode.hpp"
#include "FlexLayout/Internal/XMLLoader.hpp"
#include "FlexLayout/Internal/TreeContext.hpp"

#include "FlexLayout/Internal/NodeComponent/LayoutComponent.hpp"

namespace FlexLayout::Internal
{
	TEST(FlexBoxLayoutTest, PropergateOffset)
	{
		std::shared_ptr<FlexBoxNode> root;

		tinyxml2::XMLDocument document;
		document.Parse(R"(
			<Layout>
				<Box style="margin: 10px">
					<Box/>
				</Box>
			</Layout>
		)");

		XMLLoader{}.load(root, document);

		root->context()
			.getContext<Context::StyleContext>()
			.applyStyles(*root);

		CalculateLayout(*root, s3d::none, s3d::none);

		auto child = root->children()[0];

		auto& layout = root->getComponent<Component::LayoutComponent>();
		auto& childLayout = child->getComponent<Component::LayoutComponent>();

		layout.setPropergateOffset(true);
		layout.setLayoutOffsetRecursive(s3d::Vec2{ 0, 0 }, true);

		ASSERT_EQ(childLayout.layoutOffset(), (s3d::Vec2{ 10, 10 }));

		layout.setPropergateOffset(false);
		layout.setLayoutOffsetRecursive(s3d::Vec2{ 0, 0 }, true);

		ASSERT_EQ(childLayout.layoutOffset(), (s3d::Vec2{ 0, 0 }));
	}
}
