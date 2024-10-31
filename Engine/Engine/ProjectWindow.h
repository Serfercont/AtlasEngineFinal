#pragma once

#include "EditorWindow.h"

class ProjectWindow : public EditorWindow
{
public:
	ProjectWindow(const WindowType type, const std::string& name);
	~ProjectWindow();

	void DrawWindow() override;
};