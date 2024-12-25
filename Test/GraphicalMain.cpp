#ifdef FLEXLAYOUT_GRAPHICAL_TEST

#include <Siv3D.hpp>
#include <FlexLayout.hpp>

void Main()
{
	Window::SetStyle(WindowStyle::Sizable);

	FlexLayout::Layout layout;

	Optional<String> filePath;
	do
	{
		filePath = Dialog::OpenFile({ FileFilter::XML() });
		if (not filePath)
		{
			return;
		}
	}
	while (not layout.load(*filePath, FlexLayout::EnableHotReload::Yes));

	constexpr RectF menuRect{ 0, 0, 200, 36 * 2 };
	bool showMenu = false;
	bool fitToWindow = true;
	bool debug = false;

	while (System::Update())
	{
		if (fitToWindow)
		{
			layout.setConstraints(Scene::Rect());
		}
		else
		{
			layout.setConstraints();
		}
		layout.handleHotReload();
		layout.calculateLayout();

		layout.updateUI();
		layout.drawUI();

		if (showMenu)
		{
			showMenu = menuRect.mouseOver();
		}
		showMenu |= RectF{ 0, 0, Scene::Width(), 10 }.mouseOver();

		if (showMenu)
		{
			menuRect.draw(Palette::White);
			SimpleGUI::CheckBox(fitToWindow, U"Fit to Window", { 0, 0 });
			SimpleGUI::CheckBox(debug, U"Debug", { 0, 36 });
		}

		if (auto root = layout.document(); debug && root)
		{
			Optional<FlexLayout::Box> hoveredBox = FlexLayout::Debugger::GetHoveredBox(*root);
			if (showMenu && menuRect.mouseOver())
			{
				hoveredBox.reset();
			}

			if (hoveredBox)
			{
				FlexLayout::Debugger::DrawLayout(*hoveredBox);
				if (MouseL.down())
				{
					Console << FlexLayout::Debugger::DumpTree(*hoveredBox);
					Console << hoveredBox->getAttribute(U"style");
					Console << hoveredBox->rect();
				}
			}
		}
	}
}

#endif
