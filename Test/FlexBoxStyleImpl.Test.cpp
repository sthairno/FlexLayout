#include <gtest/gtest.h>
#include <Siv3D.hpp>
#include <FlexLayout/Internal/FlexBoxImpl.hpp>
#include <FlexLayout/Internal/Config.hpp>

namespace FlexLayout::Internal
{
	TEST(FlexBoxStyleTest, SetInlineCssText)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		root->setInlineCssText(U"top: 10px; left: 20px;");

		root->setInlineCssText(U"top: 10px;");

		auto top = root->getStyle(StylePropertyGroup::Inline, U"top");
		auto left = root->getStyle(StylePropertyGroup::Inline, U"left");

		ASSERT_EQ(top.size(), 1);
		ASSERT_EQ(top[0].getFloatValue(), 10);
		ASSERT_EQ(top[0].lengthUnit(), LengthUnit::Pixel);
		ASSERT_EQ(left.size(), 0);
	}

	TEST(FlexBoxStyleTest, GetInlineCssText)
	{
		auto root = std::make_shared<FlexBoxImpl>(U"dummy");

		root->setInlineCssText(U"top: 10px;");

		ASSERT_STREQ(Unicode::ToUTF8(root->getInlineCssText()).c_str(), "top: 10px;");
	}

	TEST(FlexBoxStyleTest, GetAndSetInlineCssText)
	{
		auto a = std::make_shared<FlexBoxImpl>(U"dummy");
		auto b = std::make_shared<FlexBoxImpl>(U"dummy");

		a->setInlineCssText(U"top: 10px; margin: 10px");
		b->setInlineCssText(a->getInlineCssText());

		ASSERT_EQ(a->getInlineCssText(), b->getInlineCssText());
		ASSERT_EQ(a->getStyle(StylePropertyGroup::Inline, U"top"), b->getStyle(StylePropertyGroup::Inline, U"top"));
		ASSERT_EQ(a->getStyle(StylePropertyGroup::Inline, U"margin"), b->getStyle(StylePropertyGroup::Inline, U"margin"));
	}

	TEST(FlexBoxStyleTest, AddedPropertyWillBeAppliedToYGNode)
	{
		auto dummy = std::make_shared<FlexBoxImpl>(U"dummy");

		dummy->setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), (YGValue{ 10, YGUnitPoint }));
	}

	TEST(FlexBoxStyleTest, ModifiedPropertyWillBeAppliedToYGNode)
	{
		auto dummy = std::make_shared<FlexBoxImpl>(U"dummy");

		dummy->setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		dummy->setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(20, LengthUnit::Pixel) });
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), (YGValue{ 20, YGUnitPoint }));
	}

	TEST(FlexBoxStyleTest, WillPropertiesContinueToBeAppliedInNextApply)
	{
		auto dummy = std::make_shared<FlexBoxImpl>(U"dummy");

		dummy->setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), (YGValue{ 10, YGUnitPoint }));
	}

	TEST(FlexBoxStyleTest, WillBeDeletedFromYGNodeIfPropertyDeleted)
	{
		auto dummy = std::make_shared<FlexBoxImpl>(U"dummy");

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), YGValueUndefined);

		dummy->setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		dummy->removeStyle(StylePropertyGroup::Inline, U"margin-top");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), YGValueUndefined);
	}

	TEST(FlexBoxStyleTest, WillPropertiesContinueToBeDeletedInNextApply)
	{
		auto dummy = std::make_shared<FlexBoxImpl>(U"dummy");

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), YGValueUndefined);

		dummy->setStyle(StylePropertyGroup::Inline, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		dummy->removeStyle(StylePropertyGroup::Inline, U"margin-top");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		ASSERT_EQ(YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop), YGValueUndefined);
	}

	TEST(FlexBoxStyleTest, PriorityGivenToPropertiesSetInParentGroup)
	{
		auto dummy = std::make_shared<FlexBoxImpl>(U"dummy");

		dummy->setStyle(StylePropertyGroup::StyleSheet, U"margin-top", std::array<Style::StyleValue, 1>{ Style::StyleValue::Length(10, LengthUnit::Pixel) });
		dummy->setInlineCssText(U"margin-top: 20px;");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		ASSERT_EQ(
			YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop),
			(YGValue{ 20, YGUnitPoint })
		);
	}

	TEST(FlexBoxStyleTest, PriorityGivenToLaterSetPropertyInSameGroup)
	{
		auto dummy = std::make_shared<FlexBoxImpl>(U"dummy");

		dummy->setInlineCssText(U"margin-top: 20px; margin: 30px");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		ASSERT_EQ(
			YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop),
			(YGValue{ 30, YGUnitPoint })
		);
	}

	TEST(FlexBoxStyleTest, AnotherPropertyWillApplyedWhenHigherPrioritizedPropertyDelete)
	{
		auto dummy = std::make_shared<FlexBoxImpl>(U"dummy");

		dummy->setInlineCssText(U"margin-top: 20px; margin: 30px");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		dummy->removeStyle(StylePropertyGroup::Inline, U"margin");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		ASSERT_EQ(
			YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop),
			(YGValue{ 20, YGUnitPoint })
		);
	}

	TEST(FlexBoxStyleTest, FontSettingsWillBeReflectedInResultsAfterApplication)
	{
		auto dummy = std::make_shared<FlexBoxImpl>(U"dummy");

		dummy->setInlineCssText(U"font-size: 10px; margin-top: 2em;");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		ASSERT_EQ(
			YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop),
			(YGValue{ 10 * 2, YGUnitPoint })
		);
	}

	TEST(FlexBoxStyleTest, FontDeletionWillBeReflectedInResultsAfterApplication)
	{
		auto dummy = std::make_shared<FlexBoxImpl>(U"dummy");

		dummy->setInlineCssText(U"font-size: 10px; margin-top: 2em;");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		dummy->removeStyle(StylePropertyGroup::Inline, U"font-size");
		FlexLayout::Internal::FlexBoxImpl::ApplyStyles(*dummy);

		ASSERT_EQ(
			YGNodeStyleGetMargin(dummy->yogaNode(), YGEdgeTop),
			(YGValue{ GetConfig().defaultTextStyle().fontSizePx * 2, YGUnitPoint })
		);
	}
}
