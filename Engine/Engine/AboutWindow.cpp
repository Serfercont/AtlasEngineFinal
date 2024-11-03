#include "AboutWindow.h"

AboutWindow::AboutWindow(const WindowType type, const std::string& name) : EditorWindow(type, name)
{
}

AboutWindow::~AboutWindow()
{
}

void AboutWindow::DrawWindow()
{
	ImGui::Begin(name.c_str());

	ImGui::End();
}