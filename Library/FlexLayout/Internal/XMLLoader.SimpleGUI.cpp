#include "XMLLoader.hpp"
#include "../SimpleGUI.hpp"

namespace FlexLayout::Internal
{
	void XMLLoader::registerSimpleGUIFactories()
	{
		registerStateFactory(
			U"SimpleGUI.Button",
			[]() -> std::unique_ptr<UIState> { return std::make_unique<SimpleGUI::Button>(); }
		);

		registerStateFactory(
			U"SimpleGUI.CheckBox",
			[]() -> std::unique_ptr<UIState> { return std::make_unique<SimpleGUI::CheckBox>(); }
		);

		registerStateFactory(
			U"SimpleGUI.RadioButtons",
			[]() -> std::unique_ptr<UIState> { return std::make_unique<SimpleGUI::RadioButtons>(); }
		);

		registerStateFactory(
			U"SimpleGUI.HorizontalRadioButtons",
			[]() -> std::unique_ptr<UIState> { return std::make_unique<SimpleGUI::HorizontalRadioButtons>(); }
		);

		registerStateFactory(
			U"SimpleGUI.ColorPicker",
			[]() -> std::unique_ptr<UIState> { return std::make_unique<SimpleGUI::ColorPicker>(); }
		);

		registerStateFactory(
			U"SimpleGUI.ListBox",
			[]() -> std::unique_ptr<UIState> { return std::make_unique<SimpleGUI::ListBox>(); }
		);

		registerStateFactory(
			U"SimpleGUI.Slider",
			[]() -> std::unique_ptr<UIState> { return std::make_unique<SimpleGUI::Slider>(); }
		);

		registerStateFactory(
			U"SimpleGUI.VerticalSlider",
			[]() -> std::unique_ptr<UIState> { return std::make_unique<SimpleGUI::VerticalSlider>(); }
		);

		registerStateFactory(
			U"SimpleGUI.TextArea",
			[]() -> std::unique_ptr<UIState> { return std::make_unique<SimpleGUI::TextArea>(); }
		);

		registerStateFactory(
			U"SimpleGUI.TextBox",
			[]() -> std::unique_ptr<UIState> { return std::make_unique<SimpleGUI::TextBox>(); }
		);
	}
}
