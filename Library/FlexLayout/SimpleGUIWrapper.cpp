#include "SimpleGUIWrapper.hpp"

namespace FlexLayout::SimpleGUI
{
	bool Button(Box& box, s3d::StringView label, bool enabled)
	{
		bool result = false;

		if (auto rect = box.rect())
		{
			result = s3d::SimpleGUI::Button(label, rect->pos, rect->w, enabled);
		}

		s3d::SizeF minSize = s3d::SimpleGUI::ButtonRegion(label, { 0, 0 }).size;

		box.setStyle(U"min-width", StyleValue::Length(static_cast<float>(minSize.x), LengthUnit::Pixel));
		box.setStyle(U"min-height", StyleValue::Length(static_cast<float>(minSize.y), LengthUnit::Pixel));

		return result;
	}

	bool Slider(Box& box, double& value, double min, double max, bool enabled)
	{
		bool result = false;

		if (auto rect = box.rect())
		{
			result = s3d::SimpleGUI::Slider(value, min, max, rect->pos, rect->w, enabled);
		}

		double minHeight = s3d::SimpleGUI::SliderRegion({ 0, 0 }).h;

		box.setStyle(U"min-height", StyleValue::Length(static_cast<float>(minHeight), LengthUnit::Pixel));

		return result;
	}

	bool VerticalSlider(Box& box, double& value, double min, double max, bool enabled)
	{
		bool result = false;

		if (auto rect = box.rect())
		{
			result = s3d::SimpleGUI::VerticalSlider(value, min, max, rect->pos, rect->h, enabled);
		}

		double minWidth = s3d::SimpleGUI::SliderRegion({ 0, 0 }).w;

		box.setStyle(U"min-width", StyleValue::Length(static_cast<float>(minWidth), LengthUnit::Pixel));

		return result;
	}

	bool CheckBox(Box& box, s3d::StringView label, bool& checked, bool enabled)
	{
		bool result = false;

		if (auto rect = box.rect())
		{
			result = s3d::SimpleGUI::CheckBox(checked, label, rect->pos, rect->w, enabled);
		}

		s3d::SizeF minSize = s3d::SimpleGUI::CheckBoxRegion(U"", { 0, 0 }).size;

		box.setStyle(U"min-width", StyleValue::Length(static_cast<float>(minSize.x), LengthUnit::Pixel));
		box.setStyle(U"min-height", StyleValue::Length(static_cast<float>(minSize.y), LengthUnit::Pixel));

		return result;
	}

	bool RadioButtons(Box& box, size_t& index, const s3d::Array<s3d::String>& options, bool enabled)
	{
		bool result = false;

		if (auto rect = box.rect())
		{
			result = s3d::SimpleGUI::RadioButtons(index, options, rect->pos, rect->w, enabled);
		}

		s3d::SizeF minSize = s3d::SimpleGUI::RadioButtonsRegion(options, { 0, 0 }).size;

		box.setStyle(U"min-width", StyleValue::Length(static_cast<float>(minSize.x), LengthUnit::Pixel));
		box.setStyle(U"min-height", StyleValue::Length(static_cast<float>(minSize.y), LengthUnit::Pixel));

		return result;
	}

	bool HorizontalRadioButtons(Box& box, size_t& index, const s3d::Array<s3d::String>& options, bool enabled)
	{
		bool result = false;

		if (auto rect = box.rect())
		{
			double itemWidth = options.empty() ? 0.0 : rect->w / options.size();
			result = s3d::SimpleGUI::HorizontalRadioButtons(index, options, rect->pos, itemWidth, enabled);
		}

		s3d::SizeF minSize = s3d::SimpleGUI::HorizontalRadioButtonsRegion(options, { 0, 0 }).size;

		box.setStyle(U"min-width", StyleValue::Length(static_cast<float>(minSize.x), LengthUnit::Pixel));
		box.setStyle(U"min-height", StyleValue::Length(static_cast<float>(minSize.y), LengthUnit::Pixel));

		return result;
	}

	bool TextBox(Box& box, s3d::TextEditState& text, const s3d::Optional<size_t>& maxChars, bool enabled)
	{
		bool result = false;

		if (auto rect = box.rect())
		{
			result = s3d::SimpleGUI::TextBox(text, rect->pos, rect->w, maxChars, enabled);
		}

		double minHeight = s3d::SimpleGUI::TextBoxRegion({ 0, 0 }).h;

		box.setStyle(U"min-height", StyleValue::Length(static_cast<float>(minHeight), LengthUnit::Pixel));

		return result;
	}

	bool TextArea(Box& box, s3d::TextAreaEditState& text, size_t maxChars, bool enabled)
	{
		bool result = false;

		if (auto rect = box.rect())
		{
			result = s3d::SimpleGUI::TextArea(text, rect->pos, rect->size, maxChars, enabled);
		}

		return result;
	}

	bool ColorPicker(Box& box, s3d::HSV& hsv, bool enabled)
	{
		bool result = false;

		if (auto rect = box.rect())
		{
			result = s3d::SimpleGUI::ColorPicker(hsv, rect->pos, enabled);
		}

		s3d::SizeF minSize = s3d::SimpleGUI::ColorPickerRegion({ 0, 0 }).size;

		box.setStyle(U"min-width", StyleValue::Length(static_cast<float>(minSize.x), LengthUnit::Pixel));
		box.setStyle(U"min-height", StyleValue::Length(static_cast<float>(minSize.y), LengthUnit::Pixel));

		return result;
	}

	bool ListBox(Box& box, s3d::ListBoxState& state, bool enabled)
	{
		bool result = false;

		if (auto rect = box.rect())
		{
			result = s3d::SimpleGUI::ListBox(state, rect->pos, rect->w, rect->h, enabled);
		}

		return result;
	}
}
