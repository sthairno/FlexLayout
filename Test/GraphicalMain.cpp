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
	}
	while (not filePath || not layout.load(*filePath, FlexLayout::EnableHotReload::Yes));

	while (System::Update())
	{
		layout.handleHotReload();
		layout.setConstraints(Scene::Rect());
		layout.calculateLayout();

		layout.updateUI();
		layout.drawUI();

		if (auto root = layout.document())
		{
			FlexLayout::Debugger::DrawHoveredBoxLayout(*root);
		}
	}
}

#endif
