#include "LengthUnit.hpp"

void FlexLayout::Style::Formatter(FormatData& formatData, const LengthUnit& value)
{
	formatData.string += U"{}"_fmt(value);
}
