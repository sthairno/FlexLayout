#define NO_S3D_USING

#include <Siv3D.hpp>
#include <yoga/Yoga.h>
#include <FlexLayout/Internal/TreeContext.hpp>
#include <FlexLayout/Internal/XMLLoader.hpp>
#include <FlexLayout/Internal/StyleProperty.hpp>
#include <FlexLayout/Internal/LabelImpl.hpp>
#include <FlexLayout.hpp>

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

	// プロパティを追加すればYGNodeに反映されるか
	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto dummy = std::make_shared<FlexBoxImpl>(context, U"dummy");

		dummy->setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop) == YGValue{ 10, YGUnitPoint }));
	}

	// プロパティを変更すればYGNodeに反映されるか
	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto dummy = std::make_shared<FlexBoxImpl>(context, U"dummy");

		dummy->setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		dummy->setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<StyleValue, 1>{ Style::StyleValue::Length(20, LengthUnit::Pixel) });
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop) == YGValue{ 20, YGUnitPoint }));
	}

	// プロパティの設定が次のApplyでも継続されるか
	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto dummy = std::make_shared<FlexBoxImpl>(context, U"dummy");

		dummy->setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop) == YGValue{ 10, YGUnitPoint }));
	}

	// プロパティを削除すればYGNodeから削除されるか
	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto dummy = std::make_shared<FlexBoxImpl>(context, U"dummy");

		assert((YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop) == YGValueUndefined));

		dummy->setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		dummy->removeStyle(StylePropertyGroup::Inline, U"margin-top");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop) == YGValueUndefined));
	}

	// プロパティの削除が次のApplyでも継続されるか
	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto dummy = std::make_shared<FlexBoxImpl>(context, U"dummy");

		assert((YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop) == YGValueUndefined));

		dummy->setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		dummy->removeStyle(StylePropertyGroup::Inline, U"margin-top");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop) == YGValueUndefined));
	}

	// 上位のグループに設定されたプロパティが優先されるか
	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto dummy = std::make_shared<FlexBoxImpl>(context, U"dummy");

		dummy->setStyle(StylePropertyGroup::StyleSheet, U"margin-top", std::array<StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		dummy->setInlineCssText(U"margin-top: 20px;");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop) == YGValue{ 20, YGUnitPoint }));
	}

	// 同一グループで後に設定されたプロパティが優先されるか
	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto dummy = std::make_shared<FlexBoxImpl>(context, U"dummy");

		dummy->setInlineCssText(U"margin-top: 20px; margin: 30px");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop) == YGValue{ 30, YGUnitPoint }));
	}

	// 優先順位が高いプロパティを削除しても、優先順位が低いプロパティが残るか
	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto dummy = std::make_shared<FlexBoxImpl>(context, U"dummy");

		dummy->setInlineCssText(U"margin-top: 20px; margin: 30px");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		dummy->removeStyle(StylePropertyGroup::Inline, U"margin");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop) == YGValue{ 20, YGUnitPoint }));
	}

	// フォントの設定が<length>を使用したプロパティに反映されるか
	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto dummy = std::make_shared<FlexBoxImpl>(context, U"dummy");

		dummy->setInlineCssText(U"font-size: 10px; margin-top: 2em;");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop) == YGValue{ 10 * 2, YGUnitPoint }));
	}

	// フォントの削除が<length>を使用したプロパティに反映されるか
	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		auto context = std::make_shared<TreeContext>();
		auto dummy = std::make_shared<FlexBoxImpl>(context, U"dummy");

		dummy->setInlineCssText(U"font-size: 10px; margin-top: 2em;");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		dummy->removeStyle(StylePropertyGroup::Inline, U"font-size");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*context);

		assert((YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop) == YGValue{ ComputedTextStyle{}.fontSizePx * 2, YGUnitPoint }));
	}

	// スタイル名のハッシュ値が一意であるか
	{
		using namespace FlexLayout::Internal;

		std::set<size_t> hashList;
		for (const auto& def : StylePropertyDefinitionList)
		{
			auto [itr, success] = hashList.insert(StyleProperty::Hash(def.first));

			assert(success);
		}
	}

	// スタイル定義のmaybeAffectToで指定されたプロパティが存在するか
	{
		using namespace FlexLayout::Internal;

		for (const auto& def : StylePropertyDefinitionList)
		{
			for (const auto& prop : def.second.maybeAffectTo)
			{
				assert(StylePropertyDefinitionList.find(prop) != StylePropertyDefinitionList.end());
			}
		}
	}

	// スタイル定義のmaybeAffectToで指定されたプロパティのmaybeAffectToが空であるか
	// (スタイルが複雑な依存関係を持っていないか)
	{
		using namespace FlexLayout::Internal;

		for (const auto& def : StylePropertyDefinitionList)
		{
			for (const auto& prop : def.second.maybeAffectTo)
			{
				assert(StylePropertyDefinitionList.at(prop).maybeAffectTo.empty());
			}
		}
	}

	// デバッガーのDumpTreeの出力が正しいか
	{
		using namespace FlexLayout;

		Layout layout{ Arg::code = UR"(
			<Layout>
				<Box id="test1" class="abc def">
					<Box id="test2">
						<Box class="abc def"/>
						<Box/>
						<Label/>
					</Box>
					<Box>
						<Box/>
						<Label/>
					</Box>
				</Box>
			</Layout>
		)" };

		const String expected = TextReader{ U"Debugger_output.txt" }.readAll();

		auto root = *layout.document();
		String output = FlexLayout::Debugger::DumpTree(root);

		Console << output;

		assert(output == expected);
	}

	// 複製元のノードのプロパティが全て複製先のノードに反映されるか
	{
		using namespace FlexLayout;
		using namespace FlexLayout::Internal;

		std::shared_ptr<FlexBoxImpl> root;

		tinyxml2::XMLDocument document;
		document.Parse(R"(
			<Layout>
				<Box id="test1" class="abc def" siv3d-font="fontasset" style="width: 100px" hoge="fuga">
					<Label id="test2">foobar</Label>
				</Box>
			</Layout>
		)");

		XMLLoader{
			std::make_shared<TreeContext>()
		}.load(root, document);

		auto clone = root->deepClone();

		assert(clone->id() == root->id());
		assert(clone->classes() == root->classes());
		assert(clone->getProperty(U"hoge") == root->getProperty(U"hoge"));
		assert(clone->fontId() == root->fontId());
		assert(clone->font() == root->font());
		assert(clone->getInlineCssText() == root->getInlineCssText());

		assert(clone->children().size() == root->children().size());
		assert(clone->children()[0]->id() == root->children()[0]->id());
		assert(clone->children()[0]->type() == NodeType::Label);
		assert(reinterpret_cast<LabelImpl*>(clone->children()[0].get())->text() == U"foobar");
	}

	Console.readLine<String>();
}
