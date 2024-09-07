#include "LengthUnit.hpp"

void FlexLayout::Formatter(s3d::FormatData& formatData, const LengthUnit& value)
{
	formatData.string += fmt::format(U"{}", value);
}
