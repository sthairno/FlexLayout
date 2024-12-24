#include <gtest/gtest.h>
#include <Siv3D.hpp>
#include <FlexLayout/Internal/Style/StyleValueParser.hpp>

namespace FlexLayout::Internal
{
	TEST(StyleValueParserTest, Color_Fail1)
	{
		auto result = ParseValue(U"random"_sv, Style::StyleValue::Type::Color);

		ASSERT_EQ(result.type(), Style::StyleValue::Type::Unspecified);
	}

	TEST(StyleValueParserTest, Color_Fail2)
	{
		auto result = ParseValue(U"#random"_sv, Style::StyleValue::Type::Color);

		ASSERT_EQ(result.type(), Style::StyleValue::Type::Unspecified);
	}

	TEST(StyleValueParserTest, Color_3Digits_LowerCase_Success)
	{
		auto result = ParseValue(U"#abc"_sv, Style::StyleValue::Type::Color);

		ASSERT_EQ(result.type(), Style::StyleValue::Type::Color);
		ASSERT_EQ(result.getColorValueUnchecked(), (s3d::Color{ 0xaa, 0xbb, 0xcc, 0xff }));
	}

	TEST(StyleValueParserTest, Color_4Digits_LowerCase_Success)
	{
		auto result = ParseValue(U"#abcd"_sv, Style::StyleValue::Type::Color);
		ASSERT_EQ(result.type(), Style::StyleValue::Type::Color);
		ASSERT_EQ(result.getColorValueUnchecked(), (s3d::Color{ 0xaa, 0xbb, 0xcc, 0xdd }));
	}

	TEST(StyleValueParserTest, Color_6Digits_LowerCase_Success)
	{
		auto result = ParseValue(U"#abcdef"_sv, Style::StyleValue::Type::Color);
		ASSERT_EQ(result.type(), Style::StyleValue::Type::Color);
		ASSERT_EQ(result.getColorValueUnchecked(), (s3d::Color{ 0xab, 0xcd, 0xef, 0xff }));
	}

	TEST(StyleValueParserTest, Color_8Digits_LowerCase_Success)
	{
		auto result = ParseValue(U"#89abcdef"_sv, Style::StyleValue::Type::Color);
		ASSERT_EQ(result.type(), Style::StyleValue::Type::Color);
		ASSERT_EQ(result.getColorValueUnchecked(), (s3d::Color{ 0x89, 0xab, 0xcd, 0xef }));
	}

	TEST(StyleValueParserTest, Color_3Digits_UpperCase_Success)
	{
		auto result = ParseValue(U"#ABC"_sv, Style::StyleValue::Type::Color);
		ASSERT_EQ(result.type(), Style::StyleValue::Type::Color);
		ASSERT_EQ(result.getColorValueUnchecked(), (s3d::Color{ 0xaa, 0xbb, 0xcc, 0xff }));
	}

	TEST(StyleValueParserTest, Color_4Digits_UpperCase_Success)
	{
		auto result = ParseValue(U"#ABCD"_sv, Style::StyleValue::Type::Color);
		ASSERT_EQ(result.type(), Style::StyleValue::Type::Color);
		ASSERT_EQ(result.getColorValueUnchecked(), (s3d::Color{ 0xaa, 0xbb, 0xcc, 0xdd }));
	}

	TEST(StyleValueParserTest, Color_6Digits_UpperCase_Success)
	{
		auto result = ParseValue(U"#ABCDEF"_sv, Style::StyleValue::Type::Color);
		ASSERT_EQ(result.type(), Style::StyleValue::Type::Color);
		ASSERT_EQ(result.getColorValueUnchecked(), (s3d::Color{ 0xab, 0xcd, 0xef, 0xff }));
	}

	TEST(StyleValueParserTest, Color_8Digits_UpperCase_Success)
	{
		auto result = ParseValue(U"#89ABCDEF"_sv, Style::StyleValue::Type::Color);
		ASSERT_EQ(result.type(), Style::StyleValue::Type::Color);
		ASSERT_EQ(result.getColorValueUnchecked(), (s3d::Color{ 0x89, 0xab, 0xcd, 0xef }));
	}
}
