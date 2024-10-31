#pragma once

#include "EditorWindow.h"

class ConsoleWindow : public EditorWindow 
{
public:
    ConsoleWindow(const WindowType type, const std::string& name);
	~ConsoleWindow();

	void DrawWindow() override;

private:
	bool showLogInfo = true;
	bool showLogWarnings = true;
	bool showLogErrors = true;

	ImVec4 infoColor = ImVec4(.25f, .5f, 1, 1);
	ImVec4 warningColor = ImVec4(1, .5f, 0, 1);
	ImVec4 errorColor = ImVec4(1, 0, 0, 1);
};