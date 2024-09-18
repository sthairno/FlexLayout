#define NO_S3D_USING

#include <Siv3D.hpp>
#include <yoga/Yoga.h>
#include <FlexLayout/Internal/TreeContext.hpp>
#include <FlexLayout/Internal/XMLLoader.hpp>
#include <FlexLayout/Internal/StyleProperty.hpp>
#include <FlexLayout/Layout.hpp>

SIV3D_SET(s3d::EngineOption::Renderer::Headless)

using namespace FlexLayout::Literals;

void Main()
{
	FontAsset::Register(U"fontasset", 10);
	FontAsset::Wait(U"fontasset");

	Font font{ 10 };
	Font fontasset = FontAsset{ U"fontasset" };

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

	{
		std::shared_ptr<FlexLayout::Internal::FlexBoxImpl> root;

		tinyxml2::XMLDocument document;
		document.Parse(R"(
			<Layout>
				<Box siv3d-font="fontasset"><Box/></Box>
			</Layout>
		)");

		FlexLayout::Internal::XMLLoader{
			std::make_shared<FlexLayout::Internal::TreeContext>()
		}.load(root, document);

		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*root->context());

		auto child = root->children()[0];

		assert((root->computedTextStyle().font == fontasset));
		assert((child->computedTextStyle().font == fontasset));
	}

	{
		auto context = std::make_shared<FlexLayout::Internal::TreeContext>();
		std::shared_ptr<FlexLayout::Internal::FlexBoxImpl> root;

		tinyxml2::XMLDocument document;
		document.Parse(R"(
			<Layout>
				<Box siv3d-font="invalid"/>
			</Layout>
		)");

		FlexLayout::Internal::XMLLoader{ context }.load(root, document);

		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((root->font().isEmpty()));
		assert((root->computedTextStyle().font == context->defaultTextStyle().font));
	}

	{
		using namespace FlexLayout::Internal;

		StylePropertyTable tbl;

		assert(tbl.group(StylePropertyGroup::Inline).size() == 0);

		auto prop = tbl.get(StylePropertyGroup::Inline, U"top");

		assert(prop);

		assert(prop->removed());

		assert(tbl.group(StylePropertyGroup::Inline).size() == 1);
	}

	{
		using namespace FlexLayout::Internal;

		StylePropertyTable tbl;

		auto prop = tbl.get(StylePropertyGroup::Inline, U"invalid");

		assert(not prop);
	}

	{
		using namespace FlexLayout::Internal;

		StylePropertyTable tbl;

		tbl.get(StylePropertyGroup::Inline, U"top");

		assert(!tbl.find(StylePropertyGroup::StyleSheet, U"top"));
	}

	{
		using namespace FlexLayout::Internal;

		StylePropertyTable tbl;

		auto a = tbl.get(StylePropertyGroup::StyleSheet, U"top");
		auto b = tbl.get(StylePropertyGroup::Inline, U"top");

		assert(tbl.find(U"top") == b);
	}

	{
		using namespace FlexLayout::Internal;

		StylePropertyTable tbl;

		tbl.get(StylePropertyGroup::Inline, U"top", true);
		tbl.get(StylePropertyGroup::Inline, U"bottom", true);
		tbl.get(StylePropertyGroup::Inline, U"left", true);
		tbl.get(StylePropertyGroup::Inline, U"top", true);

		assert(tbl.group(StylePropertyGroup::Inline)[0].keyHash() == StylePropertyDefinitionList.hash(U"bottom"));
		assert(tbl.group(StylePropertyGroup::Inline)[1].keyHash() == StylePropertyDefinitionList.hash(U"left"));
		assert(tbl.group(StylePropertyGroup::Inline)[2].keyHash() == StylePropertyDefinitionList.hash(U"top"));
	}

	{
		using namespace FlexLayout::Internal;

		StylePropertyTable tbl;

		tbl.get(StylePropertyGroup::Inline, U"top");
		tbl.get(StylePropertyGroup::Inline, U"bottom");
		tbl.get(StylePropertyGroup::Inline, U"left");

		assert(tbl.find(StylePropertyGroup::Inline, U"top"));
		assert(!tbl.find(StylePropertyGroup::Inline, U"right"));
	}

	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto root = std::make_shared<FlexBoxImpl>(context, U"dummy");

		root->setInlineCssText(U"top: 10px; left: 20px;");

		root->setInlineCssText(U"top: 10px;");

		auto top = root->getStyle(StylePropertyGroup::Inline, U"top");
		auto left = root->getStyle(StylePropertyGroup::Inline, U"left");

		assert(top.size() == 1 && top[0].getFloatValue() == 10 && top[0].lengthUnit() == LengthUnit::Pixel);
		assert(left.size() == 0);
	}

	{
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto root = std::make_shared<FlexBoxImpl>(context, U"dummy");

		root->setInlineCssText(U"top: 10px;");

		assert(root->getInlineCssText() == U"top: 10px;");
	}

	{
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto a = std::make_shared<FlexBoxImpl>(context, U"dummy");
		auto b = std::make_shared<FlexBoxImpl>(context, U"dummy");

		a->setInlineCssText(U"top: 10px; margin: 10px");
		b->setInlineCssText(a->getInlineCssText());

		assert(a->getInlineCssText() == b->getInlineCssText());
		assert(
			a->getStyle(StylePropertyGroup::Inline, U"top") == b->getStyle(StylePropertyGroup::Inline, U"top") &&
			a->getStyle(StylePropertyGroup::Inline, U"margin") == b->getStyle(StylePropertyGroup::Inline, U"margin")
		);
	}

	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto root = std::make_shared<FlexBoxImpl>(context, U"dummy");

		root->setStyle(StylePropertyGroup::StyleSheet, U"margin-top", std::array<StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		root->setInlineCssText(U"margin-top: 20px;");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(root->yogaNode(), YGEdgeTop) == YGValue{ 20, YGUnitPoint }));

		root->setInlineCssText(U"margin: 30px;");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(root->yogaNode(), YGEdgeTop) == YGValue{ 30, YGUnitPoint }));
		
		root->setInlineCssText(U"margin-top: 40px;");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(root->yogaNode(), YGEdgeTop) == YGValue{ 40, YGUnitPoint }));
	}
}
