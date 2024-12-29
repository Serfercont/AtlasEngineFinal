    
#pragma once
#include "EditorWindow.h"
#include "ModuleTimeManager.h"
#include "imgui.h"

class TimeManagerEditorWindow : public EditorWindow {
public:
    TimeManagerEditorWindow();
    ~TimeManagerEditorWindow() = default;   

    void DrawWindow() override;

private:
    void DrawPlaybackControls();
    void DrawTimeInfo();

    const float TOOLBAR_WIDTH = 120.0f;
    const float TOOLBAR_HEIGHT = 45.0f;
    const float BUTTON_WIDTH = 30.0f;

    ImGuiWindowFlags windowFlags = 0;
};  