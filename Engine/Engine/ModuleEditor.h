#pragma once

#include "Module.h"
#include "GameObject.h"
#include "EditorWindow.h"

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
	void PreferencesWindow();
	void Docking();
	void MainMenuBar();

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
	//Preferences
	ImVec4 dataTextColor = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);

	std::list<EditorWindow*> editorWindows;
};