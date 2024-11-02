#pragma once

#include "EditorWindow.h"

#include <filesystem>
#include <vector>
#include <string>

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

private:
    std::filesystem::path currentPath;
	std::filesystem::path selectedPath;
    std::vector<std::filesystem::directory_entry> directoryContents;

    bool showPathBar = false;

    bool smallSelected = false;
    bool largeSelected = true;

	float columnWidth = 100.0f;
	float maxTextWidth = 80.0f;
    float smallIconSize = 16.0f;
	float largeIconSize = 64.0f;
};