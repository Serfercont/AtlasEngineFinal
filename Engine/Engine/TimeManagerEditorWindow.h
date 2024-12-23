#pragma once
#include "EditorWindow.h"
#include "ModuleTimeManager.h"
#include <string>

class TimeManagerEditorWindow : public EditorWindow {
public:
    TimeManagerEditorWindow() : EditorWindow(WindowType::PREFERENCES, "Time Manager") {}
    ~TimeManagerEditorWindow() {}

    void DrawWindow() override {
        if (!IsEnabled()) return;

        ImGui::Begin(GetName().c_str(), &enabled);

        if (ImGui::Button("Play")) {
            ModuleTimeManager::GetInstance()->Play();
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
            ModuleTimeManager::GetInstance()->Pause();
        } 
        ImGui::SameLine();
        if (ImGui::Button("Stop")) {
            ModuleTimeManager::GetInstance()->Stop();
        }

        float gameTime = ModuleTimeManager::GetInstance()->GetTime();
        ImGui::Text("Game Time: %.2f seconds", gameTime);

        float realTime = ModuleTimeManager::GetInstance()->GetRealTimeSinceStartup();
        ImGui::Text("Real Time Since Startup: %.2f seconds", realTime);

        float timeScale = ModuleTimeManager::GetInstance()->GetTimeScale();
        if (ImGui::SliderFloat("Time Scale", &timeScale, 0.0f, 2.0f)) {
            ModuleTimeManager::GetInstance()->SetTimeScale(timeScale);
        }

        ImGui::End();
    }
};
