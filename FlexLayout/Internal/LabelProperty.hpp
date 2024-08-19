#pragma once
#include <Siv3D.hpp>

namespace FlexLayout::Internal
{
	enum class FontScaleType
	{
		Inherit,
		Pixel,
		Percent,
		Em,
		Rem
	};

	enum class TextAlign
	{
		Inherit,
		Left,
		Right,
		Center
	};

	struct LabelProperty
	{
		Optional<Font> font;

		double fontScale = 1.0;
		FontScaleType fontScaleType = FontScaleType::Inherit;

		Optional<double> lineHeight;

		TextAlign textAlign = TextAlign::Inherit;
	};

	bool LoadStyleToLabelProperty(LabelProperty& node, const StringView key, const StringView value);
}
