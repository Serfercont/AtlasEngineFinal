#include "TimeManagerEditorWindow.h"
#include "App.h"

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
        if (ModuleTimeManager::GetInstance()->GetCurrentState() == GameState::GAME_MODE) {
            ImGui::Text("Modo: GAME");
        }
        else {
            ImGui::Text("Modo: EDITOR");
        }

        ImGui::Separator();
        ImGui::Text("Playback Controls");
        DrawPlaybackControls();
        DrawTimeInfo();
    }
    ImGui::End();
}

void TimeManagerEditorWindow::DrawPlaybackControls()
{
    GameState currentState = ModuleTimeManager::GetInstance()->GetCurrentState();
    bool isPlaying = (currentState == GameState::GAME_MODE && !ModuleTimeManager::GetInstance()->IsPaused());
    bool isPaused = ModuleTimeManager::GetInstance()->IsPaused() &&
        (currentState == GameState::GAME_MODE);
    bool isStopped = (currentState == GameState::EDITOR_MODE);

   
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));


    float totalButtonsWidth = BUTTON_WIDTH * 3 + ImGui::GetStyle().ItemSpacing.x * 2;
    float cursorPosX = (ImGui::GetWindowSize().x - totalButtonsWidth) * 0.5f;
    ImGui::SetCursorPosX(cursorPosX);

    // Play
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
            app->editor->gameWindow->StartGame();
        }
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Play");
    if (isPlaying) ImGui::PopStyleColor(3);
    ImGui::SameLine();

    // Pause
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

    // Stop
    if (isStopped) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
    }

    if (ImGui::Button("X", ImVec2(BUTTON_WIDTH, 0))) {
        ModuleTimeManager::GetInstance()->Stop();
		app->editor->gameWindow->StopGame();
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Stop");
    if (isStopped) ImGui::PopStyleColor(3);

    ImGui::PopStyleVar(2); 
}


void TimeManagerEditorWindow::DrawTimeInfo() {
    ImGui::Spacing();
    ImGui::Separator();

    const ModuleTimeManager* timeManager = ModuleTimeManager::GetInstance();

    // Estilo para el fondo del panel de información
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.2f, 1.0f));

    // Información de tiempos
    ImGui::BeginGroup();
    ImGui::Text("Game Time: %.2f s", timeManager->GetTime());
    ImGui::Text("Real Time: %.2f s", timeManager->GetRealTimeSinceStartup());
    ImGui::Text("Delta Time: %.3f ms", timeManager->GetDeltaTime() * 1000.0f);
    ImGui::Text("Time Scale: %.2fx", timeManager->GetTimeScale());
    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Separator();

    // Estado del juego con colores
    if (timeManager->IsPaused()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.4f, 1.0f));
        ImGui::Text("Estado: PAUSADO");
        ImGui::PopStyleColor();
    }
    else if (timeManager->GetCurrentState() == GameState::GAME_MODE) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
        ImGui::Text("Estado: EJECUTANDO");
        ImGui::PopStyleColor();
    }
    else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
        ImGui::Text("Estado: EDITOR");
        ImGui::PopStyleColor();
    }

    ImGui::PopStyleColor(); // Pop del FrameBg
}