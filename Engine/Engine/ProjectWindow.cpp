#include "ProjectWindow.h"

ProjectWindow::ProjectWindow(const WindowType type, const std::string& name) : EditorWindow(type, name), currentPath("Assets")
{
    UpdateDirectoryContent();
}

ProjectWindow::~ProjectWindow()
{
}

void ProjectWindow::DrawWindow()
{
    ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        std::vector<std::string> pathParts = GetPathParts();

        for (size_t i = 0; i < pathParts.size(); ++i)
        {
            if (i == pathParts.size() - 1)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
            }

            if (ImGui::MenuItem(pathParts[i].c_str()))
            {
                std::filesystem::path newPath = std::filesystem::path("Assets");
                for (size_t j = 1; j <= i; ++j)
                {
                    newPath /= pathParts[j];
                }

                if (std::filesystem::exists(newPath) && std::filesystem::is_directory(newPath))
                {
                    currentPath = newPath;
                    UpdateDirectoryContent();
                }
            }

            if (i == pathParts.size() - 1)
            {

                ImGui::PopStyleColor();
            }

            if (i < pathParts.size() - 1)
            {
                ImGui::Text(">");
            }
        }

        ImGui::EndMenuBar();
    }

    for (const auto& entry : directoryContents)
    {
        if (entry.is_directory() && !entry.path().filename().empty())
        {
            if (ImGui::Selectable(entry.path().filename().string().c_str(), false))
            {
                currentPath = entry.path();
                UpdateDirectoryContent();
            }
        }
        else
        {
            ImGui::Text(entry.path().filename().string().c_str());
        }
    }

    ImGui::End();
}

void ProjectWindow::UpdateDirectoryContent()
{
    directoryContents.clear();
    for (const auto& entry : std::filesystem::directory_iterator(currentPath))
    {
        directoryContents.push_back(entry);
    }
}

std::vector<std::string> ProjectWindow::GetPathParts() const
{
    std::vector<std::string> parts;
    for (const auto& part : currentPath)
    {
        parts.push_back(part.string());
    }
    return parts;
}