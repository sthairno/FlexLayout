#pragma once
#include "../detail/Common.hpp"
#include "LengthUnit.hpp"
#include "StyleEnums.hpp"

namespace FlexLayout::Style
{
	class StyleValue
	{
	public:

		enum class Type : int8_t
		{
			None = 0,
			Auto,
			Integer,
			Enum,
			Ratio,
			Percentage,
			Number,
			Length
		};

	public:

		StyleValue() = default;

		StyleValue(const StyleValue&) = default;

		StyleValue& operator=(const StyleValue&) = default;

		StyleValue(StyleValue&&) = default;

		StyleValue& operator=(StyleValue&&) = default;

		bool operator==(const StyleValue& other) const noexcept;

		bool operator!=(const StyleValue& other) const noexcept
		{
			return !(*this == other);
		}

		constexpr explicit operator bool() const noexcept
		{
			return m_valueType != Type::None;
		}

	public:

		Type type() const noexcept { return m_valueType; }

		template<typename EnumType>
		Optional<EnumType> getEnumValue() const noexcept
		{
			static_assert(std::is_nothrow_convertible_v<std::underlying_type_t<EnumType>, int32_t>);
			using traits = detail::style_enum_traits<EnumType>;

			if (m_valueType != Type::Enum)
			{
				return none;
			}

			if (m_enumTypeId != StyleEnumTypeID::Unknown && m_enumTypeId != traits::id)
			{
				return none;
			}

			return static_cast<EnumType>(m_intValue);
		}

		template<typename EnumType>
		EnumType getEnumValueUnchecked() const noexcept
		{
			static_assert(std::is_nothrow_convertible_v<std::underlying_type_t<EnumType>, int32_t>);
			return static_cast<EnumType>(m_intValue);
		}

		Optional<int32_t> getIntValue() const noexcept
		{
			if (m_valueType == Type::Integer)
			{
				return m_intValue;
			}

			return none;
		}

		int32_t getIntValueUnchecked() const noexcept { return m_intValue; }

		Optional<float> getFloatValue() const noexcept
		{
			switch (m_valueType)
			{
			case Type::Ratio:
			case Type::Percentage:
			case Type::Number:
			case Type::Length:
				return m_floatValue;
			}
			return none;
		}

		float getFloatValueUnchecked() const noexcept { return m_floatValue; }

		LengthUnit lengthUnit() const noexcept { return m_lengthUnit; }

	public:

		inline static StyleValue None() noexcept
		{
			return StyleValue{ Type::None };
		}

		inline static StyleValue Auto() noexcept
		{
			return StyleValue{ Type::Auto };
		}

		inline static StyleValue Integer(int32_t value) noexcept
		{
			return StyleValue{ Type::Integer, value };
		}

		template<class EnumType>
		inline static StyleValue Enum(EnumType value)
		{
			static_assert(std::is_nothrow_convertible_v<std::underlying_type_t<EnumType>, int32_t>);

			return StyleValue{
				Type::Enum,
				static_cast<int32_t>(static_cast<std::underlying_type_t<EnumType>>(value)),
				detail::style_enum_traits<EnumType>::id
			};
		}

		inline static StyleValue Ratio(float value)
		{
			assert(std::isfinite(value) && value >= 0.0f);
			return StyleValue{ Type::Ratio, value };
		}

		inline static StyleValue Ratio(float width, float height)
		{
			assert(std::isfinite(width) && std::isfinite(height));
			assert(width >= 0.0F && height >= 0.0F);
			return StyleValue{ Type::Ratio, width / height };
		}

		inline static StyleValue Percentage(float value)
		{
			assert(std::isfinite(value));
			return StyleValue{ Type::Percentage, value };
		}

		inline static StyleValue Number(float value)
		{
			assert(std::isfinite(value));
			return StyleValue{ Type::Number, value };
		}

		inline static StyleValue Length(float length, LengthUnit unit = LengthUnit::Unspecified)
		{
			assert(std::isfinite(length) && length >= 0.0F);
			return StyleValue{ Type::Length, length, unit };
		}

	private:

		union
		{
			float m_floatValue;

			int32_t m_intValue;
		};

		Type m_valueType = Type::None;

		StyleEnumTypeID m_enumTypeId = StyleEnumTypeID::Unknown;

		LengthUnit m_lengthUnit = LengthUnit::Unspecified;

		explicit StyleValue(Type type) noexcept
			: m_valueType(type)
			, m_floatValue(0.0F) { }

		explicit StyleValue(Type type, float value) noexcept
			: m_valueType(type)
			, m_floatValue(value) { }

		explicit StyleValue(Type type, float value, LengthUnit unit) noexcept
			: m_valueType(type)
			, m_floatValue(value)
			, m_lengthUnit(unit) { }

		explicit StyleValue(Type type, int32_t value) noexcept
			: m_valueType(type)
			, m_intValue(value) { }

		explicit StyleValue(Type type, int32_t value, StyleEnumTypeID enumid) noexcept
			: m_valueType(type)
			, m_intValue(value)
			, m_enumTypeId(enumid) { }
	};
}
