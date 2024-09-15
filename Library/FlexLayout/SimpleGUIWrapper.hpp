#pragma once
#include <Siv3D/SimpleGUI.hpp>
#include "Box.hpp"

namespace FlexLayout::SimpleGUI
{
	[[nodiscard]]
	bool Button(Box& box, s3d::StringView label, bool enabled = true);

	[[nodiscard]]
	bool Slider(Box& box, double& value, double min = 0.0, double max = 1.0, bool enabled = true);

	[[nodiscard]]
	bool VerticalSlider(Box& box, double& value, double min = 0.0, double max = 1.0, bool enabled = true);

	[[nodiscard]]
	bool CheckBox(Box& box, s3d::StringView label, bool& checked, bool enabled = true);

	[[nodiscard]]
	bool RadioButtons(Box& box, size_t& index, const s3d::Array<s3d::String>& options, bool enabled = true);

	[[nodiscard]]
	bool HorizontalRadioButtons(Box& box, size_t& index, const s3d::Array<s3d::String>& options, bool enabled = true);

	[[nodiscard]]
	bool TextBox(Box& box, s3d::TextEditState& text, const s3d::Optional<size_t>& maxChars = s3d::unspecified, bool enabled = true);

	[[nodiscard]]
	bool TextArea(Box& box, s3d::TextAreaEditState& text, size_t maxChars = s3d::SimpleGUI::PreferredTextAreaMaxChars, bool enabled = true);

	[[nodiscard]]
	bool ColorPicker(Box& box, s3d::HSV& hsv, bool enabled = true);

	[[nodiscard]]
	bool ListBox(Box& box, s3d::ListBoxState& state, bool enabled = true);
}
