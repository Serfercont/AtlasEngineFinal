#pragma once

#include "EditorWindow.h"
#include "ModuleWindow.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class SceneWindow : public EditorWindow
{
public:
	SceneWindow(const WindowType type, const std::string& name);
	~SceneWindow();

	void DrawWindow() override;
private:
	void HandleMousePicking() const;
public:
	ImVec2 windowSize = ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT);
};