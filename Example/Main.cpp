#include <Siv3D.hpp>
#include <FlexLayout.hpp>

// <数値>_px や <数値>_ch などのリテラルを使えるようにする
using namespace FlexLayout::Literals;

void Main()
{
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetBackground(Palette::White);

	// レイアウト本体
	FlexLayout::Layout layout{ U"Layout.xml", FlexLayout::EnableHotReload::Yes };

	// ボックスの描画関数
	std::function<void(const FlexLayout::Box&)> drawBox;
	drawBox = [&](const FlexLayout::Box& box) {
		auto borderAreaRect = box.borderAreaRect();
		auto contentAreaRect = box.contentAreaRect();

		// 描画
		box.drawFrame(Palette::Gray);
		if (auto label = box.asLabel())
		{
			label->draw(Palette::Black);
		}

		// 子ボックスを再帰的に描画
		for (const auto& child : box.children())
		{
			drawBox(child);
		}
	};

	double sliderValue = 0.5;

	int cloneId = 0;

	while (System::Update())
	{
		// レイアウトの更新
		layout.updateAll(Scene::Rect());

		if (auto document = layout.document())
		{
			drawBox(*document);

			auto button = document->getElementById(U"button");
			auto slider = document->getElementById(U"slider");

			// Box[id="button"] と Box[id="slider"] に対応するボタンとスライダーを配置

			if (button)
			{
				FlexLayout::SimpleGUI::Button(*button, U"Button");
			}

			if (slider)
			{
				FlexLayout::SimpleGUI::Slider(*slider, sliderValue);
			}

			auto cloneTarget = document->getElementById(U"clone-target");
			auto cloneButton = document->getElementById(U"clone-button");
			auto resetButton = document->getElementById(U"reset-button");

			if (cloneButton &&
				FlexLayout::SimpleGUI::Button(*cloneButton, U"Clone", cloneTarget.has_value()))
			{
				auto cloneTemplate = *document->getElementById(U"clone-template");
				auto clone = cloneTemplate.cloneNode(true);

				// 非表示のボックスを表示させる
				clone.unsetStyle(U"display");
				clone.removeAttribute(U"id");

				// IDのラベルを更新
				clone.getElementById(U"clone-label")->asLabel()->setText(Format(cloneId++));

				cloneTarget->appendChild(clone);
			}

			if (resetButton &&
				FlexLayout::SimpleGUI::Button(*resetButton, U"Reset", cloneTarget.has_value() && cloneTarget->hasChildNodes()))
			{
				cloneId = 0;
				cloneTarget->removeChildren();
			}

			FlexLayout::Debugger::DrawHoveredBoxLayout(*document);
		}
	}
}
