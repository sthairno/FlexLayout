#define NO_S3D_USING

#include <Siv3D.hpp>
#include <yoga/Yoga.h>
#include <FlexLayout/Internal/TreeContext.hpp>
#include <FlexLayout/Internal/XMLLoader.hpp>
#include <FlexLayout/Layout.hpp>

SIV3D_SET(s3d::EngineOption::Renderer::Headless)

using namespace FlexLayout::Literals;

void Main()
{
	{
		bool called = false;
		auto callback = [&](FlexLayout::Box& box) -> void
			{
				called = true;
			};
		FlexLayout::Layout layout(callback);

		assert(not called);

		assert(layout.load(s3d::Arg::code = U"<Layout></Layout>"));

		assert(called);
	}

	{
		std::shared_ptr<FlexLayout::Internal::FlexBoxImpl> root;

		tinyxml2::XMLDocument document;
		document.Parse(R"(
			<Layout>
				<Box style="margin: 10px">
					<Box/>
				</Box>
			</Layout>
		)");

		FlexLayout::Internal::XMLLoader{
			std::make_shared<FlexLayout::Internal::TreeContext>()
		}.load(root, document);

		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*root->context());
		FlexLayout::Internal::FlexBoxImpl::CalculateLayout(*root, s3d::none, s3d::none);

		auto child = root->children()[0];

		root->setPropergateOffset(true);
		root->setLayoutOffsetRecursive(s3d::Vec2{ 0, 0 }, true);
		assert((child->layoutOffset() == s3d::Vec2{ 10, 10 }));

		root->setPropergateOffset(false);
		root->setLayoutOffsetRecursive(s3d::Vec2{ 0, 0 }, true);
		assert((child->layoutOffset() == s3d::Vec2{ 0, 0 }));
	}
}
