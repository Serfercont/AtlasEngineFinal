#pragma once
#include "EditorWindow.h"
#include "ModuleTimeManager.h"
#include <glm/glm.hpp>

class GameWindow : public EditorWindow {
public:
    GameWindow(const WindowType type, const std::string& name);
    ~GameWindow();

    void DrawWindow() override;

    void StartGame();

    void StopGame();

    ImVec2 windowSize = ImVec2(0, 0);
    bool IsMouseInside() const { return isMouseInside; }
    ImVec2 GetLastMousePosition() const { return lastMousePosition; }

private:
    void UpdateMouseState();
    bool isMouseInside = false;
    ImVec2 lastMousePosition = ImVec2(0, 0);
    bool isWindowFocused = false;
    ModuleTimeManager* timeManager;
private:
	bool isPaused = true;
};