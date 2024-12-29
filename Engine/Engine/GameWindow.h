#pragma once
#include "EditorWindow.h"
#include <glm/glm.hpp>

class GameWindow : public EditorWindow {
public:
    GameWindow(const WindowType type, const std::string& name);
    ~GameWindow();

    void DrawWindow() override;

    ImVec2 windowSize = ImVec2(0, 0);
    bool IsMouseInside() const { return isMouseInside; }
    ImVec2 GetLastMousePosition() const { return lastMousePosition; }

private:
    void UpdateMouseState();
    bool isMouseInside = false;
    ImVec2 lastMousePosition = ImVec2(0, 0);
    bool isWindowFocused = false;
};