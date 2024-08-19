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
			Unspecified = 0,
			None,
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

		bool operator==(const StyleValue& other) const noexcept;

		bool operator!=(const StyleValue& other) const noexcept
		{
			return !(*this == other);
		}

		constexpr explicit operator bool() const noexcept
		{
			return m_valueType != Type::Unspecified;
		}

		friend void Formatter(FormatData& formatData, const StyleValue& value);

	public:

		Type type() const noexcept { return m_valueType; }

		template<typename EnumType>
		Optional<EnumType> getEnumValue() const noexcept
		{
			auto id = detail::style_enum_id_from_type<EnumType>::value;

			if (m_valueType != Type::Enum || m_enumTypeId != id)
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

		EnumTypeId enumTypeId() const noexcept { return m_enumTypeId; }

		LengthUnit lengthUnit() const noexcept { return m_lengthUnit; }

		String toString() const;

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
			return StyleValue{
				Type::Enum,
				static_cast<int32_t>(value),
				detail::style_enum_id_from_type<EnumType>::value
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

		static StyleValue Parse(StringView str, Array<Type> acceptedType = { Type::Unspecified });

		inline static StyleValue Parse(StringView str, Type acceptedType = Type::Unspecified)
		{
			return Parse(str, Array<Type>{ acceptedType });
		}

	private:

		union
		{
			float m_floatValue;

			std::int32_t m_intValue;
		};

		Type m_valueType = Type::Unspecified;

		EnumTypeId m_enumTypeId = -1;

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

		explicit StyleValue(Type type, int32_t value, std::int8_t enumid) noexcept
			: m_valueType(type)
			, m_intValue(value)
			, m_enumTypeId(enumid) { }
	};
}

template <>
struct SIV3D_HIDDEN fmt::formatter<FlexLayout::Style::StyleValue, s3d::char32>
{
	std::u32string tag;

	auto parse(fmt::basic_format_parse_context<s3d::char32>& ctx)
	{
		return s3d::detail::GetFormatTag(tag, ctx);
	}

	template <typename FormatContext>
	auto format(const FlexLayout::Style::StyleValue& value, FormatContext& ctx)
	{
		using namespace FlexLayout::Style;
		
		switch (value.type())
		{
		case StyleValue::Type::Unspecified:
			return format_to(ctx.out(), U"");
		case StyleValue::Type::None:
			return format_to(ctx.out(), U"none");
		case StyleValue::Type::Auto:
			return format_to(ctx.out(), U"auto");
		case StyleValue::Type::Integer:
			return format_to(ctx.out(), U"{}", value.getIntValueUnchecked());
		case StyleValue::Type::Enum:
			return format_to(ctx.out(), U"{}", FlexLayout::Style::detail::GetValueName(value.enumTypeId(), value.getIntValueUnchecked()));
		case StyleValue::Type::Ratio:
			return format_to(ctx.out(), U"{}", value.getFloatValueUnchecked());
		case StyleValue::Type::Percentage:
			return format_to(ctx.out(), U"{}%", value.getFloatValueUnchecked());
		case StyleValue::Type::Number:
			return format_to(ctx.out(), U"{}", value.getFloatValueUnchecked());
		case StyleValue::Type::Length:
			return format_to(ctx.out(), U"{}", value.getFloatValueUnchecked()); // TODO: unit
		}
	}
};
