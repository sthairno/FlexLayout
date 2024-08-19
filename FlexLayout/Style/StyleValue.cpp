#include "StyleValue.hpp"

namespace FlexLayout::Style
{
	bool StyleValue::operator==(const StyleValue& other) const noexcept
	{
		if (m_valueType != other.m_valueType)
		{
			return false;
		}

		switch (m_valueType)
		{
		case Type::None: return true;
		case Type::Auto: return true;
		case Type::Enum: return m_enumTypeId == other.m_enumTypeId && m_intValue == other.m_intValue;
		case Type::Ratio: return m_floatValue == other.m_floatValue;
		case Type::Percentage: return m_floatValue == other.m_floatValue;
		case Type::Number: return m_floatValue == other.m_floatValue;
		case Type::Length: return m_floatValue == other.m_floatValue && m_lengthUnit == other.m_lengthUnit;
		}

		return false;
	}

	void Formatter(FormatData& formatData, const StyleValue& value)
	{
		formatData.string.append(U"{}"_fmt(value));
	}

	String StyleValue::toString() const
	{
		return U"{}"_fmt(*this);
	}
}
