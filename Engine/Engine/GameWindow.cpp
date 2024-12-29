#include "GameWindow.h"
#include "App.h"

GameWindow::GameWindow(const WindowType type, const std::string& name)
    : EditorWindow(type, name)
{
    timeManager = app->timeManager;
}

GameWindow::~GameWindow()
{
}

void GameWindow::UpdateMouseState()
{
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();

    isMouseInside = (mousePos.x >= windowPos.x && mousePos.x <= windowPos.x + windowSize.x &&
        mousePos.y >= windowPos.y && mousePos.y <= windowPos.y + windowSize.y);

    if (isMouseInside)
    {
        lastMousePosition = ImVec2(
            mousePos.x - windowPos.x,
            mousePos.y - windowPos.y
        );
    }

    isWindowFocused = ImGui::IsWindowFocused();
}

void GameWindow::DrawWindow()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    UpdateMouseState();

    const ImVec2 newWindowSize = ImGui::GetContentRegionAvail();
    if (windowSize.x != newWindowSize.x || windowSize.y != newWindowSize.y)
    {
        windowSize = newWindowSize;
        // Necesitaremos un framebuffer específico para la Game Window
        if (timeManager != nullptr && timeManager->isPlaying)
        {
            app->renderer3D->OnGameResize(windowSize.x, windowSize.y);
        }
    }

    // Solo mostramos el contenido si estamos en modo Play
    if (timeManager != nullptr && timeManager->isPlaying)
    {
        ImGui::Image(
            (void*)(intptr_t)app->renderer3D->gameTextureId,
            windowSize,
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        // Opcional: Mostrar controles de reproducción
        ImGui::SetCursorPos(ImVec2(10, 10));
        if (ImGui::Button("Stop"))
        {
			timeManager->Stop();
            //app->StopGame();
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause"))
        {
			timeManager->Pause();
            //app->PauseGame();
        }
    }
    else
    {
        // Mostrar mensaje cuando no está en Play
        ImVec2 textSize = ImGui::CalcTextSize("Game View (Not Playing)");
        ImVec2 centerPos = ImVec2(
            (windowSize.x - textSize.x) * 0.5f,
            (windowSize.y - textSize.y) * 0.5f
        );

        ImGui::SetCursorPos(centerPos);
        ImGui::Text("Game View (Not Playing)");
    }

    ImGui::End();
    ImGui::PopStyleVar();
}