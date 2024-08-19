#include <Siv3D.hpp>
#include "FlexLayout/Style/StyleValue.hpp"
#include "FlexLayout/Style/StyleValueParser.hpp"

void Main()
{
	Console << U"Test 1:";

	// テスト：シリアライズ単体

	Array<FlexLayout::Style::StyleValue> values{
		FlexLayout::Style::StyleValue(),
		FlexLayout::Style::StyleValue::None(),
		FlexLayout::Style::StyleValue::Auto(),
		FlexLayout::Style::StyleValue::Integer(100),
		FlexLayout::Style::StyleValue::Enum(FlexLayout::Style::AlignContent::SpaceBetween),
		FlexLayout::Style::StyleValue::Ratio(0.5),
		FlexLayout::Style::StyleValue::Ratio(1, 3),
		FlexLayout::Style::StyleValue::Percentage(80),
		FlexLayout::Style::StyleValue::Number(123),
		FlexLayout::Style::StyleValue::Length(456, FlexLayout::Style::LengthUnit::Pixel)
	};

	for (const auto value : values)
	{
		Console.write(U"|");
		Console << value;
	}

	Console << U"\nTest 2:";

	// テスト：シリアライズ->デシリアライズ

	Array<FlexLayout::Style::StyleValue> restoredValues;

	for (const auto value : values | std::views::drop(1))
	{
		if (value.type() == FlexLayout::Style::StyleValue::Type::Enum)
		{
			restoredValues.push_back(FlexLayout::Style::ParseValue(value.toString(), value.enumTypeId()));
			continue;
		}
		else
		{
			restoredValues.push_back(FlexLayout::Style::ParseValue(value.toString(), value.type()));
			continue;
		}
	}

	for (const auto value : restoredValues)
	{
		Console.write(U"|");
		Console << value;
	}

	Console.readLine<String>();
}
