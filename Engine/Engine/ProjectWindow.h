#pragma once

#include "EditorWindow.h"

#include <filesystem>
#include <vector>
#include <string>

#include <GL/glew.h>

class ProjectWindow : public EditorWindow
{
public:
    ProjectWindow(const WindowType type, const std::string& name);
    ~ProjectWindow();

    void DrawWindow() override;

    void UpdateDirectoryContent();
    std::vector<std::string> GetPathParts() const;
    
    void DrawFoldersTree(const std::filesystem::path& directoryPath);
    void DrawDirectoryContents();
    void DrawMenuBar();
    void DrawSelectionBar();

    GLuint LoadTexture(const std::string& filePath);

private:
    std::filesystem::path currentPath;
	std::filesystem::path selectedPath;
    std::vector<std::filesystem::directory_entry> directoryContents;

    GLuint folderIcon;
	GLuint openFolderIcon;
    GLuint fileIcon;

    bool showPathBar = false;
};