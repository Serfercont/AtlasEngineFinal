#pragma once

#include "EditorWindow.h"
#include "GameObject.h"

class HierarchyWindow : public EditorWindow
{
public:
	HierarchyWindow(const WindowType type, const std::string& name);
	~HierarchyWindow();

	void DrawWindow() override;

	void HierarchyTree(GameObject* node, bool isRoot = false, const char* searchText = "");
	bool ValidateHierarchy(GameObject* node);
	bool HasCircularReference(GameObject* node, std::vector<GameObject*>& visited);
	void DropNodeToRoot(GameObject* draggedNode);
	bool IsDescendant(GameObject* potentialChild, GameObject* parent);
	bool FilterNode(GameObject* node, const char* searchText);

private:
	char searchInput[256] = "";
	char inputName[256] = "GameObject";
	ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
};