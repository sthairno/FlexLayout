#include "LengthUnit.hpp"

void FlexLayout::Formatter(FormatData& formatData, const LengthUnit& value)
{
	formatData.string += U"{}"_fmt(value);
}
