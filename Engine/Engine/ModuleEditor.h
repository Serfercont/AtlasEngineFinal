#pragma once

#include "Module.h"
#include "GameObject.h"

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
	void PreferencesWindow();
	void Docking();
	void MainMenuBar();

	void HierarchyTree(GameObject* node, bool isRoot = false, const char* searchText = "");
	bool FilterNode(GameObject* node, const char* searchText);

public:
	GameObject* selectedGameObject = nullptr;

	bool drawTextures = true;
	bool wireframe = false;
	bool shadedWireframe = false;	
	
	// Normals settings
	float vertexNormalLength = 0.1f;
	float faceNormalLength = 0.1f;
	glm::vec3 vertexNormalColor = glm::vec3(0, 1, 0);
	glm::vec3 faceNormalColor = glm::vec3(1, 0, 0);

private:
	//Inspector
	char searchInput[256] = "";
	char inputName[256] = "GameObject";
	ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
	bool isEditingInspector = false;

	//Console
	bool showLogInfo = true;
	bool showLogWarnings = true;
	bool showLogErrors = true;

	ImVec4 infoColor = ImVec4(.25f, .5f, 1, 1);
	ImVec4 warningColor = ImVec4(1, .5f, 0, 1);
	ImVec4 errorColor = ImVec4(1, 0, 0, 1);
};