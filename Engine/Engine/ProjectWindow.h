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
	void ConfigureColumns();
	void DrawTile(const std::filesystem::directory_entry& entry, bool& shouldBreakLoop);
	void DrawListItem(const std::filesystem::directory_entry& entry, bool& shouldBreakLoop);
	void DrawColumnItem(const std::filesystem::directory_entry& entry, bool& shouldBreakLoop);
    std::string GetTruncatedFilename(const std::string& filename, float maxTextWidth);
    void HandleItemClick(const std::filesystem::directory_entry& entry, bool& shouldBreakLoop);

private:
    std::filesystem::path currentPath;
	std::filesystem::path selectedPath;
    std::vector<std::filesystem::directory_entry> directoryContents;

    bool showPathBar = false;

    bool isItemSelected = false;

    bool listSelected = false;
    bool tilesSelected = true;
	bool columnSelected = false;

	bool oneColumnSelected = false;
	bool twoColumnsSelected = true;

	float columnWidth = 100.0f;
	float columnHeight = 95.0f;
	float maxTextWidth = 80.0f;
    float smallIconSize = 16.0f;
	float largeIconSize = 64.0f;
};