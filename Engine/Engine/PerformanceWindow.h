#pragma once

#include "EditorWindow.h"
#include <windows.h>
#include "GL/glew.h"

class PerformanceWindow : public EditorWindow
{
public:
	PerformanceWindow(const WindowType type, const std::string& name);
	~PerformanceWindow();

	void DrawWindow() override;

private:
	ImVec4 dataTextColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);

	// CPU
	std::string cpuName;
	SYSTEM_INFO sysInfo;

	// GPU
	const GLubyte* vendor;
	const GLubyte* renderer;
	const GLubyte* version;
	const GLubyte* glsl;
};