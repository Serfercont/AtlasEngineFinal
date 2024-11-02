#include "ConsoleWindow.h"
#include "App.h"
#include "Logger.h"

ConsoleWindow::ConsoleWindow(const WindowType type, const std::string& name) : EditorWindow(type, name)
{
}

ConsoleWindow::~ConsoleWindow()
{
}

void ConsoleWindow::DrawWindow()
{
    ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushStyleColor(ImGuiCol_CheckMark, infoColor);
    ImGui::Checkbox("##Info", &showLogInfo);
    ImGui::SameLine();
	ImGui::Image((ImTextureID)app->importer->icons.infoIcon, ImVec2(20, 20));
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_CheckMark, warningColor);
    ImGui::Checkbox("##Warning", &showLogWarnings);
    ImGui::SameLine();
	ImGui::Image((ImTextureID)app->importer->icons.warningIcon, ImVec2(20, 20));
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_CheckMark, errorColor);
    ImGui::Checkbox("##Error", &showLogErrors);
    ImGui::SameLine();
	ImGui::Image((ImTextureID)app->importer->icons.errorIcon, ImVec2(20, 20));
    ImGui::PopStyleColor();

    ImGui::SameLine();
    if (ImGui::Button("Clear"))
        logger.Clear();

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 10));

    std::string logType;
    ImVec4 logColor;

    for (const auto& log : logger.GetLogs())
    {
        switch (log.type)
        {
        case LogType::LOG_INFO:
            logType = "[INFO]";
            logColor = infoColor;
            if (!showLogInfo) continue;
            break;

        case LogType::LOG_WARNING:
            logType = "[WARNING]";
            logColor = warningColor;
            if (!showLogWarnings) continue;
            break;

        case LogType::LOG_ERROR:
            logType = "[ERROR]";
            logColor = errorColor;
            if (!showLogErrors) continue;
            break;
        }

        ImGui::PushStyleColor(ImGuiCol_Text, logColor);
        ImGui::Text(logType.c_str());
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::Text(log.message.c_str());
    }

    ImGui::PopStyleVar();

    ImGui::End();
}