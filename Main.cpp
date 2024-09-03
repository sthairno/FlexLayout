#include <Siv3D.hpp>
#include "FlexLayout/FlexLayout.hpp"

void DrawFlexBox(const FlexLayout::FlexBox& box, int depth = 0)
{
	const auto rect = box.marginAreaRect();
	const Color color = HSV{ depth * 40, 0.8, 0.9 }.toColorF();

	// 四角形を描画
	if (rect)
	{
		rect->draw(color).drawFrame(1, Palette::Gray);
	}

	// テキストを描画
	box.draw(Palette::Black);

	for (const auto& child : box.children())
	{
		DrawFlexBox(child, depth + 1);
	}
}

void Main()
{
	// レイアウト本体
	FlexLayout::FlexLayout layout{ U"Layout.xml", FlexLayout::EnableHotReload::Yes };

	// idが指定されている要素はホットリロードしても参照が保持されます
	auto document = *layout.document();
	auto buttonBox = document.getElementById(U"button");

	while (System::Update())
	{
		// レイアウトの更新
		layout.update(Scene::Rect());

		// ツリーの四角形を描画
		DrawFlexBox(document);

		// Box[id="button"]に合わせてボタンを描画
		if (buttonBox)
		{
			if (auto rect = buttonBox->contentAreaRect())
			{
				SimpleGUI::Button(U"Hello, Yoga!", rect->pos, rect->w);
			}
		}
	}
}
