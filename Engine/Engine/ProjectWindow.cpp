#include "ProjectWindow.h"

#include "IL/il.h"

ProjectWindow::ProjectWindow(const WindowType type, const std::string& name) : EditorWindow(type, name), currentPath("Assets")
{
    UpdateDirectoryContent();

    folderIcon = LoadTexture("Assets/Icons/folder.png");
    openFolderIcon = LoadTexture("Assets/Icons/open_folder.png");
    fileIcon = LoadTexture("Assets/Icons/file.png");
}

ProjectWindow::~ProjectWindow()
{
}

void ProjectWindow::DrawWindow()
{
    ImGui::Begin(name.c_str());

    ImGui::Columns(2, "ProjectColumns");

    ImGui::BeginChild("Folders", ImVec2(0, 0), ImGuiChildFlags_None);
    DrawFoldersTree("Assets");
	ImGui::EndChild();

    ImGui::NextColumn();

    ImGui::BeginChild("Assets", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), ImGuiChildFlags_None, ImGuiWindowFlags_MenuBar);
    DrawDirectoryContents();
    ImGui::EndChild();

    ImGui::BeginChild("SelectionBar", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_MenuBar);
    DrawSelectionBar();
    ImGui::EndChild();

    ImGui::Columns(1);

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

void ProjectWindow::DrawFoldersTree(const std::filesystem::path& directoryPath)
{
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool hasSubfolders = false;

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
    {
        if (entry.is_directory())
        {
            hasSubfolders = true;
            break;
        }
    }

    if (!hasSubfolders)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }

    bool open = ImGui::TreeNodeEx(("##" + directoryPath.string()).c_str(), nodeFlags);

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        currentPath = directoryPath;
        UpdateDirectoryContent();
    }

    void* icon = hasSubfolders && open ? (void*)(intptr_t)openFolderIcon : (void*)(intptr_t)folderIcon;

    ImGui::SameLine();
    ImGui::Image(icon, ImVec2(16, 16));

    if (ImGui::IsItemClicked())
    {
        currentPath = directoryPath;
        UpdateDirectoryContent();
    }

    ImGui::SameLine();
    ImGui::TextUnformatted(directoryPath.filename().string().c_str());

    if (open)
    {
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
        {
            if (entry.is_directory())
            {
                DrawFoldersTree(entry.path());
            }
        }
        ImGui::TreePop();
    }
}

void ProjectWindow::DrawDirectoryContents()
{
    DrawMenuBar();

    bool isItemSelected = false;

    for (const auto& entry : directoryContents)
    {
        bool verifyIfSelected = true;

        if (entry.is_directory() && !entry.path().filename().empty())
        {
            

            ImGui::Image((void*)(intptr_t)folderIcon, ImVec2(16, 16));
            ImGui::SameLine();

            if (ImGui::Selectable(entry.path().filename().string().c_str(), false))
            {
                currentPath = entry.path();
                UpdateDirectoryContent();
                verifyIfSelected = false;
            }
            if (verifyIfSelected && ImGui::IsItemHovered())
            {
                selectedPath = entry.path();
                isItemSelected = true;
                showPathBar = true;
            }
        }
		else if (entry.is_regular_file())
        {
            ImGui::Image((void*)(intptr_t)fileIcon, ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::Selectable(entry.path().filename().string().c_str()))
            {
                verifyIfSelected = false;
            }

            if (ImGui::IsItemHovered())
            {
                selectedPath = entry.path();
                isItemSelected = true;
                showPathBar = true;
            }
        }
    }

    if (!isItemSelected)
    {
        showPathBar = false;
    }
}

void ProjectWindow::DrawMenuBar()
{
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
}

void ProjectWindow::DrawSelectionBar()
{
    if (showPathBar && !selectedPath.empty())
    {
        if (ImGui::BeginMenuBar())
        {
            ImGui::Image((void*)(intptr_t)folderIcon, ImVec2(16, 16));
            ImGui::SameLine();
            ImGui::Text("%s", selectedPath.string().c_str());

            ImGui::EndMenuBar();
        }
    }
}

GLuint ProjectWindow::LoadTexture(const std::string& filePath)
{
    ilClearColour(255, 255, 255, 255);

    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    if (ilLoadImage(filePath.c_str()) == IL_FALSE) 
    {
        ilDeleteImages(1, &imageID);
        return 0;
    }

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), 
        ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ilDeleteImages(1, &imageID);

    return textureID;
}