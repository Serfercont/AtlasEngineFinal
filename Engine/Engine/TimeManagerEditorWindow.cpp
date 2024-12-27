#include "TimeManagerEditorWindow.h"

TimeManagerEditorWindow::TimeManagerEditorWindow()
    : EditorWindow(WindowType::PREFERENCES, "Time Manager")
{
    this->windowFlags = ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoCollapse;
}

void TimeManagerEditorWindow::DrawWindow()
{
    if (!IsEnabled()) return;

    ImGui::SetNextWindowSize(ImVec2(TOOLBAR_WIDTH + 20, TOOLBAR_HEIGHT + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.9f);

    if (ImGui::Begin(GetName().c_str(), &enabled, this->windowFlags))
    {
        ImGui::Text("Playback Controls");
        DrawPlaybackControls();
        DrawTimeInfo();
    }
    ImGui::End();
}

void TimeManagerEditorWindow::DrawPlaybackControls()
{
    // Estilo de los botones
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

    // Centrado de botones
    float totalButtonsWidth = BUTTON_WIDTH * 3 + ImGui::GetStyle().ItemSpacing.x * 2;
    float cursorPosX = (ImGui::GetWindowSize().x - totalButtonsWidth) * 0.5f;
    ImGui::SetCursorPosX(cursorPosX);

    // Botón Play
    bool isPlaying = !ModuleTimeManager::GetInstance()->IsPaused();
    if (isPlaying) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    }

    if (ImGui::Button(">", ImVec2(BUTTON_WIDTH, 0))) { 
        if (isPlaying) {
            ModuleTimeManager::GetInstance()->Pause();
        }
        else {
            ModuleTimeManager::GetInstance()->Play();
        }
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Play");
    if (isPlaying) ImGui::PopStyleColor(3);
    ImGui::SameLine();

    // Botón Pause
    bool isPaused = ModuleTimeManager::GetInstance()->IsPaused() &&
        ModuleTimeManager::GetInstance()->GetTime() > 0;
    if (isPaused) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.7f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.6f, 0.2f, 1.0f));
    }

    if (ImGui::Button("II", ImVec2(BUTTON_WIDTH, 0))) { 
        ModuleTimeManager::GetInstance()->Pause();
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Pause");
    if (isPaused) ImGui::PopStyleColor(3);
    ImGui::SameLine();

    // Botón Stop
    if (ImGui::Button("X", ImVec2(BUTTON_WIDTH, 0))) { 
        ModuleTimeManager::GetInstance()->Stop();
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Stop"); 

    ImGui::PopStyleVar(2);
}

void TimeManagerEditorWindow::DrawTimeInfo()
{
    ImGui::Spacing();
    ImGui::Separator();

    
    float gameTime = ModuleTimeManager::GetInstance()->GetTime();
    ImGui::Text("Game Time: %.2f s", gameTime);

    float realTime = ModuleTimeManager::GetInstance()->GetRealTimeSinceStartup();
    ImGui::Text("Real Time: %.2f s", realTime);

    
    float timeScale = ModuleTimeManager::GetInstance()->GetTimeScale();
    if (ImGui::SliderFloat("Time Scale", &timeScale, 0.0f, 2.0f)) {
        ModuleTimeManager::GetInstance()->SetTimeScale(timeScale);
    }
}
