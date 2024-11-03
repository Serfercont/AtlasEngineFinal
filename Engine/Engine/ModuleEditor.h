#pragma once

#include "Module.h"
#include "GameObject.h"
#include "EditorWindow.h"

#include "ConsoleWindow.h"
#include "HierarchyWindow.h"
#include "InspectorWindow.h"
#include "ProjectWindow.h"
#include "PerformanceWindow.h"
#include "PreferencesWindow.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <list>

class ModuleEditor : public Module
{
public:
	ModuleEditor(App* app);
	~ModuleEditor();

	bool Awake();
	bool CleanUp();

	void DrawEditor();
	void Docking();
	void MainMenuBar();

public:
	GameObject* selectedGameObject = nullptr;

	ConsoleWindow* consoleWindow = nullptr;
	HierarchyWindow* hierarchyWindow = nullptr;
	InspectorWindow* inspectorWindow = nullptr;
	ProjectWindow* projectWindow = nullptr;
	PerformanceWindow* performanceWindow = nullptr;
	PreferencesWindow* preferencesWindow = nullptr;

private:

	std::list<EditorWindow*> editorWindows;
};