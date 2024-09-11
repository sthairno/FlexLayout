﻿#include <Siv3D.hpp>
#include <FlexLayout.hpp>

// <数値>_px や <数値>_ch などのリテラルを使えるようにする
using namespace FlexLayout::Literals;

namespace FlexLayout::SimpleGUI
{
	bool Button(StringView label, FlexLayout::Box& box, bool enabled = true)
	{
		bool result = false;

		if (auto rect = box.rect())
		{
			result = s3d::SimpleGUI::Button(label, rect->pos, rect->w, enabled);
		}

		SizeF minSize = s3d::SimpleGUI::ButtonRegion(label, { 0, 0 }).size;

		box.setStyle(U"min-width", StyleValue::Length(minSize.x, LengthUnit::Pixel));
		box.setStyle(U"min-height", StyleValue::Length(minSize.y, LengthUnit::Pixel));

		return result;
	}
}

void Main()
{
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetBackground(Palette::White);

	// レイアウト本体
	FlexLayout::Layout layout{ U"Layout.html", FlexLayout::EnableHotReload::Yes };

	// カーソルが乗っているボックス
	Optional<FlexLayout::Box> hoveredBox;
	
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

		// カーソル判定
		if (auto rect = box.borderAreaRect(); rect && rect->mouseOver())
		{
			hoveredBox = box;
		}

		// 子ボックスを再帰的に描画
		for (const auto& child : box.children())
		{
			drawBox(child);
		}
	};

	constexpr double InspectorColorAlpha = 0.7;

	while (System::Update())
	{
		hoveredBox.reset();

		// レイアウトの更新
		layout.update(Scene::Rect());

		if (auto document = layout.document())
		{
			drawBox(*document);

			if (auto button = document->getElementById(U"button"))
			{
				FlexLayout::SimpleGUI::Button(U"Button", *button);
			}
		}

		// マウスが乗っているボックスの各領域を描画
		if (hoveredBox && hoveredBox->offset())
		{
			auto marginAreaRect = *hoveredBox->marginAreaRect();
			hoveredBox->margin().drawPadding(marginAreaRect, ColorF{ Palette::Darkorange, InspectorColorAlpha });

			auto borderAreaRect = *hoveredBox->borderAreaRect();
			hoveredBox->border().drawPadding(borderAreaRect, ColorF{ Palette::Gold, InspectorColorAlpha });

			auto paddingAreaRect = *hoveredBox->paddingAreaRect();
			hoveredBox->padding().drawPadding(paddingAreaRect, ColorF{ Palette::Yellowgreen, InspectorColorAlpha });

			auto contentAreaRect = *hoveredBox->contentAreaRect();
			contentAreaRect.draw(ColorF{ Palette::Skyblue, InspectorColorAlpha });
		}
	}
}
