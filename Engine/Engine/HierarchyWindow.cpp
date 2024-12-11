#include "HierarchyWindow.h"
#include "App.h"
#include <vector>
#include <algorithm>

HierarchyWindow::HierarchyWindow(const WindowType type, const std::string& name) : EditorWindow(type, name)
{
}

HierarchyWindow::~HierarchyWindow()
{
}

void HierarchyWindow::DrawWindow()
{
	ImGui::Begin(name.c_str());

	UpdateMouseState();

	if (ImGui::Button("+", ImVec2(20, 20)))
	{
		ImGui::OpenPopup("GameObject");
	}

	if (ImGui::BeginPopup("GameObject"))
	{
		if (ImGui::MenuItem("Create Empty"))
		{
			app->scene->CreateGameObject("GameObject", app->scene->root);
			app->editor->selectedGameObject = app->scene->root->children.back();
		}
		if (ImGui::BeginMenu("3D Object"))
		{
			const char* objectNames[] = { "Cube", "Sphere", "Capsule", "Cylinder" };
			const char* basePath = "Engine/Primitives/";
			const char* extension = ".fbx";

			for (const char* name : objectNames)
			{
				std::string fullPath = std::string(basePath) + name + extension;

				if (ImGui::MenuItem(name))
				{
					Resource* resource = app->resources->FindResourceInLibrary(fullPath, ResourceType::MODEL);
					if (!resource)
						resource = app->importer->ImportFileToLibrary(fullPath, ResourceType::MODEL);

					app->importer->modelImporter->LoadModel(resource, app->scene->root);
					app->editor->selectedGameObject = app->scene->root->children.back();
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	ImGui::SameLine();

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputTextWithHint("##Search", "Search", searchInput, 256);

	ImGui::BeginGroup();

	HierarchyTree(app->scene->root, true, searchInput);

	ImVec2 availableSize = ImGui::GetContentRegionAvail();

	ImGui::Dummy(availableSize);

	ImGui::EndGroup();

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FILE_PATH"))
		{
			const char* droppedFilePath = static_cast<const char*>(payload->Data);
			app->importer->ImportFile(droppedFilePath, true);
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::End();
}

void HierarchyWindow::HierarchyTree(GameObject* node, bool isRoot, const char* searchText)
{
    // Validaciones previas
    if (!node) return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

    if (isRoot)
    {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }

    if (node->children.empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    bool isSelected = (app->editor->selectedGameObject == node);

    if (isSelected)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (!node->isActive)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    }

    if (FilterNode(node, searchText))
    {
        bool isOpen = ImGui::TreeNodeEx(node, flags, node->name.c_str());

        if (ImGui::IsItemClicked())
        {
            if (app->editor->selectedGameObject && app->editor->selectedGameObject->isEditing)
            {
                app->editor->selectedGameObject->isEditing = false;
            }
            app->editor->selectedGameObject = node;
        }

        // Drag and Drop Source
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            ImGui::SetDragDropPayload("HIERARCHY_NODE", &node, sizeof(GameObject*));
            ImGui::Text("Moving %s", node->name.c_str());
            ImGui::EndDragDropSource();
        }

        // Drag and Drop Target
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_NODE"))
            {
                GameObject* draggedNode = *(GameObject**)payload->Data;

                // Validaciones más robustas
                if (draggedNode && draggedNode != node &&
                    !IsDescendant(draggedNode, node) &&  // No permitir mover a un descendiente
                    node != draggedNode->parent)        // No mover si ya es su padre actual
                {
                    // Eliminar del padre anterior
                    if (draggedNode->parent)
                    {
                        auto& siblings = draggedNode->parent->children;
                        siblings.erase(
                            std::remove(siblings.begin(), siblings.end(), draggedNode),
                            siblings.end()
                        );
                    }

                    // Establecer nuevo padre
                    draggedNode->parent = node;
                    node->children.push_back(draggedNode);
                }
            }
            ImGui::EndDragDropTarget();
        }

        // Renderizar hijos recursivamente
        if (isOpen && !node->children.empty())
        {
            for (auto child : node->children)
            {
                HierarchyTree(child, false, searchText);
            }
            ImGui::TreePop();
        }

        if (!node->isActive)
        {
            ImGui::PopStyleColor();
        }
    }
    else
    {
        // Filtrar recursivamente
        for (auto child : node->children)
        {
            HierarchyTree(child, false, searchText);
        }
    }
}

bool HierarchyWindow::ValidateHierarchy(GameObject* node)
{
    if (!node) return false;

    // Comprobar referencias circulares
    std::vector<GameObject*> visited;
    return !HasCircularReference(node, visited);
}

bool HierarchyWindow::HasCircularReference(GameObject* node, std::vector<GameObject*>& visited)
{
    if (!node) return false;

    // Comprueba si el nodo ya está en el vector
    if (std::find(visited.begin(), visited.end(), node) != visited.end())
        return true; // Referencia circular detectada

    visited.push_back(node); // Añade el nodo a la lista de visitados

    for (auto child : node->children)
    {
        if (HasCircularReference(child, visited))
            return true;
    }

    visited.pop_back(); // Elimina el nodo al retroceder en la recursión
    return false;
}
// Nueva función para manejar nodos sueltos (mover a la raíz)
void HierarchyWindow::DropNodeToRoot(GameObject* draggedNode)
{
    // Si tiene un padre, desconéctalo
    if (draggedNode->parent)
    {
        auto& siblings = draggedNode->parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), draggedNode), siblings.end());
        draggedNode->parent = nullptr; // Mover a la raíz
    }

    app->scene->root->children.push_back(draggedNode);
}

// Nueva función: Comprueba si `potentialChild` es descendiente de `parent`
bool HierarchyWindow::IsDescendant(GameObject* potentialChild, GameObject* parent)
{
    if (!parent) return false;

    for (auto child : parent->children)
    {
        if (child == potentialChild || IsDescendant(potentialChild, child))
        {
            return true;
        }
    }
    return false;
}


bool HierarchyWindow::FilterNode(GameObject* node, const char* searchText)
{
	std::string nodeNameLower = node->name;
	std::transform(nodeNameLower.begin(), nodeNameLower.end(), nodeNameLower.begin(), ::tolower);

	std::string searchTextLower = searchText;
	std::transform(searchTextLower.begin(), searchTextLower.end(), searchTextLower.begin(), ::tolower);

	return nodeNameLower.find(searchTextLower) != std::string::npos;
}