#include "LabelProperty.hpp"

bool FlexLayout::detail::LoadStyleToLabelProperty(LabelProperty& prop, const StringView key, const StringView value)
{
	if (key == U"-siv3d-font")
	{
		if (value == U"inherit")
		{
			prop.font.reset();
			return true;
		}

		prop.font = FontAsset{ value };
		return true;
	}
	else if (key == U"font-size")
	{
		if (value == U"inherit")
		{
			prop.fontScaleType = FontScaleType::Inherit;
			return true;
		}

		try
		{
			if (value.ends_with(U"%"))
			{
				prop.fontScale = Parse<double>(value.substr(0, value.length() - 1));
				prop.fontScaleType = FontScaleType::Percent;
			}
			else if (value.ends_with(U"em"))
			{
				prop.fontScale = Parse<double>(value.substr(0, value.length() - 2));
				prop.fontScaleType = FontScaleType::Em;
			}
			else if (value.ends_with(U"rem"))
			{
				prop.fontScale = Parse<double>(value.substr(0, value.length() - 3));
				prop.fontScaleType = FontScaleType::Rem;
			}
			else if (value.ends_with(U"px"))
			{
				prop.fontScale = Parse<double>(value.substr(0, value.length() - 2));
				prop.fontScaleType = FontScaleType::Pixel;
			}
			else
			{
				prop.fontScale = Parse<double>(value);
				prop.fontScaleType = FontScaleType::Pixel;
			}
		}
		catch (ParseError)
		{ }

		return true;
	}
	else if (key == U"line-height")
	{
		if (value == U"inherit")
		{
			prop.lineHeight.reset();
			return true;
		}

		if (auto v = ParseOpt<double>(value))
		{
			prop.lineHeight = *v;
		}
		return true;
	}
	else if (key == U"text-align")
	{
		if (value == U"inherit")
		{
			prop.textAlign = TextAlign::Inherit;
		}
		else if (value == U"start")
		{
			prop.textAlign = TextAlign::Left;
		}
		else if (value == U"end")
		{
			prop.textAlign = TextAlign::Right;
		}
		else if (value == U"left")
		{
			prop.textAlign = TextAlign::Left;
		}
		else if (value == U"right")
		{
			prop.textAlign = TextAlign::Right;
		}
		else if (value == U"center")
		{
			prop.textAlign = TextAlign::Center;
		}
		return true;
	}

	return false;
}
