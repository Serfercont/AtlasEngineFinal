#pragma once

#include "EditorWindow.h"

class AboutWindow : public EditorWindow
{
public:
	AboutWindow(const WindowType type, const std::string& name);
	~AboutWindow();

	void DrawWindow() override;
};