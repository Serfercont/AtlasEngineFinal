#include "SceneWindow.h"
#include "App.h"

SceneWindow::SceneWindow(const WindowType type, const std::string& name) : EditorWindow(type, name)
{
}

SceneWindow::~SceneWindow()
{
}

void SceneWindow::DrawWindow()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        if (ImGui::BeginMenu("Shading Mode"))
        {
            static int w = 0;
            for (int n = 0; n < 3; n++)
            {
                const char* names[] = { "Shaded", "Wireframe", "Shaded Wireframe" };
                if (ImGui::Selectable(names[n], w == n))
                {
                    w = n;
                    if (n == 0)
                    {
                        app->editor->preferencesWindow->wireframe = false;
                        app->editor->preferencesWindow->shadedWireframe = false;
                    }
                    else if (n == 1)
                    {
                        app->editor->preferencesWindow->wireframe = true;
                        app->editor->preferencesWindow->shadedWireframe = false;
                    }
                    else if (n == 2)
                    {
                        app->editor->preferencesWindow->wireframe = false;
                        app->editor->preferencesWindow->shadedWireframe = true;
                    }
                }
            }

            ImGui::EndMenu();
        }
        ImGui::PopStyleVar();

        ImGui::EndMenuBar();
    }

    if (ImGui::IsWindowHovered())
        app->camera->isMouseInside = true;
    else if (app->camera->isMouseInside)
		app->camera->isMouseInside = false;

    ImVec2 textureSize = ImVec2(app->window->width, app->window->height);
    ImVec2 windowSize = ImGui::GetWindowSize();

    ImVec2 uv0 = ImVec2((textureSize.x - windowSize.x) / (2 * textureSize.x),
        (textureSize.y + windowSize.y) / (2 * textureSize.y));
    ImVec2 uv1 = ImVec2((textureSize.x + windowSize.x) / (2 * textureSize.x),
        (textureSize.y - windowSize.y) / (2 * textureSize.y));

    ImGui::Image((void*)(intptr_t)app->renderer3D->fboTexture, windowSize, uv0, uv1);

    ImGui::End();
    ImGui::PopStyleVar();
}
