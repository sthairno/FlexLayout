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
	FlexLayout::Layout templateLayout{ U"Template.xml", FlexLayout::EnableHotReload::Yes };

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

	int cloneId = 0;

	auto document = layout.document();
	auto button = *document->getElementById(U"button")->as<FlexLayout::SimpleGUI::Button>();
	auto slider = *document->getElementById(U"slider")->as<FlexLayout::SimpleGUI::Slider>();
	auto cloneTarget = *document->getElementById(U"clone-target");
	auto cloneButton = *document->getElementById(U"clone-button")->as<FlexLayout::SimpleGUI::Button>();
	auto resetButton = *document->getElementById(U"reset-button")->as<FlexLayout::SimpleGUI::Button>();

	while (System::Update())
	{
		layout.updateAll(Scene::Rect());
		layout.drawUI();

		if (auto document = layout.document())
		{
			drawBox(*document);

			if (cloneButton->clicked())
			{
				auto clone = templateLayout.document()->cloneNode(true);

				// IDのラベルを更新
				clone.getElementById(U"label")->setTextContent(Format(cloneId++));

				cloneTarget.appendChild(clone);
			}

			if (resetButton->clicked())
			{
				cloneId = 0;
				cloneTarget.removeChildren();
			}

			FlexLayout::Debugger::DrawHoveredBoxLayout(*document);
		}
	}
}
