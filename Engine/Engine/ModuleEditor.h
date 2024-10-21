#pragma once

#include "Module.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor(App* app);
	~ModuleEditor();

	bool Awake();
	bool CleanUp();

	void DrawEditor();
	void HierarchyWindow();
	void InspectorWindow();
	void ConsoleWindow();
	void ProjectWindow();
	void Docking();
	void MainMenuBar();
};