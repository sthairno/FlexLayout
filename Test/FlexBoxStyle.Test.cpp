#include <gtest/gtest.h>
#include <Siv3D.hpp>
#include <FlexLayout/Internal/FlexBoxNode.hpp>
#include <FlexLayout/Internal/Config.hpp>
#include <FlexLayout/Internal/TreeContext.hpp>

#include <FlexLayout/Internal/NodeComponent/StyleComponent.hpp>

namespace FlexLayout::Internal
{
	TEST(FlexBoxStyleTest, SetInlineCssText)
	{
		auto root = std::make_shared<FlexBoxNode>();
		auto& style = root->getComponent<Component::StyleComponent>();

		style.setInlineCssText(U"top: 10px; left: 20px;");

		style.setInlineCssText(U"top: 10px;");

		auto top = style.getStyle(StylePropertyGroup::Inline, U"top");
		auto left = style.getStyle(StylePropertyGroup::Inline, U"left");

		ASSERT_EQ(top.size(), 1);
		ASSERT_EQ(top[0].getFloatValue(), 10);
		ASSERT_EQ(top[0].lengthUnit(), LengthUnit::Pixel);
		ASSERT_EQ(left.size(), 0);
	}

	TEST(FlexBoxStyleTest, GetInlineCssText)
	{
		auto root = std::make_shared<FlexBoxNode>();
		auto& style = root->getComponent<Component::StyleComponent>();

		style.setInlineCssText(U"top: 10px;");

		ASSERT_STREQ(Unicode::ToUTF8(style.getInlineCssText()).c_str(), "top: 10px;");
	}

	TEST(FlexBoxStyleTest, GetAndSetInlineCssText)
	{
		auto a = std::make_shared<FlexBoxNode>();
		auto b = std::make_shared<FlexBoxNode>();
		auto& aStyle = a->getComponent<Component::StyleComponent>();
		auto& bStyle = b->getComponent<Component::StyleComponent>();

		aStyle.setInlineCssText(U"top: 10px; margin: 10px");
		bStyle.setInlineCssText(aStyle.getInlineCssText());

		ASSERT_EQ(aStyle.getInlineCssText(), bStyle.getInlineCssText());
		ASSERT_EQ(aStyle.getStyle(StylePropertyGroup::Inline, U"top"), bStyle.getStyle(StylePropertyGroup::Inline, U"top"));
		ASSERT_EQ(aStyle.getStyle(StylePropertyGroup::Inline, U"margin"), bStyle.getStyle(StylePropertyGroup::Inline, U"margin"));
	}

	TEST(FlexBoxStyleTest, AddedPropertyWillBeAppliedToYGNode)
	{
		auto dummy = std::make_shared<FlexBoxNode>();
		auto& style = dummy->getComponent<Component::StyleComponent>();

		style.setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), (YGValue{ 10, YGUnitPoint }));
	}

	TEST(FlexBoxStyleTest, ModifiedPropertyWillBeAppliedToYGNode)
	{
		auto dummy = std::make_shared<FlexBoxNode>();
		auto& style = dummy->getComponent<Component::StyleComponent>();

		style.setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		style.setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(20, LengthUnit::Pixel) });
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), (YGValue{ 20, YGUnitPoint }));
	}

	TEST(FlexBoxStyleTest, WillPropertiesContinueToBeAppliedInNextApply)
	{
		auto dummy = std::make_shared<FlexBoxNode>();
		auto& style = dummy->getComponent<Component::StyleComponent>();

		style.setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), (YGValue{ 10, YGUnitPoint }));
	}

	TEST(FlexBoxStyleTest, WillBeDeletedFromYGNodeIfPropertyDeleted)
	{
		auto dummy = std::make_shared<FlexBoxNode>();
		auto& style = dummy->getComponent<Component::StyleComponent>();

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), YGValueUndefined);

		style.setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		style.removeStyle(StylePropertyGroup::Inline, U"margin-top");
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), YGValueUndefined);
	}

	TEST(FlexBoxStyleTest, WillPropertiesContinueToBeDeletedInNextApply)
	{
		auto dummy = std::make_shared<FlexBoxNode>();
		auto& style = dummy->getComponent<Component::StyleComponent>();

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), YGValueUndefined);

		style.setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		style.removeStyle(StylePropertyGroup::Inline, U"margin-top");
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), YGValueUndefined);
	}

	TEST(FlexBoxStyleTest, PriorityGivenToPropertiesSetInParentGroup)
	{
		auto dummy = std::make_shared<FlexBoxNode>();
		auto& style = dummy->getComponent<Component::StyleComponent>();

		style.setStyle(StylePropertyGroup::StyleSheet, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		style.setInlineCssText(U"margin-top: 20px;");
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		ASSERT_EQ(
			YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop),
			(YGValue{ 20, YGUnitPoint })
		);
	}

	TEST(FlexBoxStyleTest, PriorityGivenToLaterSetPropertyInSameGroup)
	{
		auto dummy = std::make_shared<FlexBoxNode>();
		auto& style = dummy->getComponent<Component::StyleComponent>();

		style.setInlineCssText(U"margin-top: 20px; margin: 30px");
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		ASSERT_EQ(
			YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop),
			(YGValue{ 30, YGUnitPoint })
		);
	}

	TEST(FlexBoxStyleTest, AnotherPropertyWillApplyedWhenHigherPrioritizedPropertyDelete)
	{
		auto dummy = std::make_shared<FlexBoxNode>();
		auto& style = dummy->getComponent<Component::StyleComponent>();

		style.setInlineCssText(U"margin-top: 20px; margin: 30px");
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		style.removeStyle(StylePropertyGroup::Inline, U"margin");
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		ASSERT_EQ(
			YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop),
			(YGValue{ 20, YGUnitPoint })
		);
	}

	TEST(FlexBoxStyleTest, FontSettingsWillBeReflectedInResultsAfterApplication)
	{
		auto dummy = std::make_shared<FlexBoxNode>();
		auto& style = dummy->getComponent<Component::StyleComponent>();

		style.setInlineCssText(U"font-size: 10px; margin-top: 2em;");
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		ASSERT_EQ(
			YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop),
			(YGValue{ 10 * 2, YGUnitPoint })
		);
	}

	TEST(FlexBoxStyleTest, FontDeletionWillBeReflectedInResultsAfterApplication)
	{
		auto dummy = std::make_shared<FlexBoxNode>();
		auto& style = dummy->getComponent<Component::StyleComponent>();

		style.setInlineCssText(U"font-size: 10px; margin-top: 2em;");
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		style.removeStyle(StylePropertyGroup::Inline, U"font-size");
		dummy->context().getContext<Context::StyleContext>().applyProperties(*dummy);

		ASSERT_EQ(
			YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop),
			(YGValue{ GetConfig().defaultTextStyle().fontSizePx * 2, YGUnitPoint })
		);
	}
}
