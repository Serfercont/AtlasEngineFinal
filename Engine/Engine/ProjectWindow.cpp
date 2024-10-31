#include "ProjectWindow.h"

ProjectWindow::ProjectWindow(const WindowType type, const std::string& name) : EditorWindow(type, name)
{
}

ProjectWindow::~ProjectWindow()
{
}

void ProjectWindow::DrawWindow()
{
	ImGui::Begin(name.c_str());

	ImGui::End();
}