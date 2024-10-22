#include "ComponentMaterial.h"
#include "GameObject.h"

#include <windows.h>
#include <shellapi.h>
#include <algorithm> 

ComponentMaterial::ComponentMaterial(GameObject* gameObject) : Component(gameObject, ComponentType::MATERIAL), textureId(-1)
{
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::Update()
{

}

void ComponentMaterial::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (textureId != -1)
		{
			ImGui::Text("Path: %s", texturePath);
			ImGui::Text("Texture Size: %i x %i", textureWidth, textureHeight);
			ImGui::Image((int*)textureId, ImVec2(200, 200));

			if (ImGui::MenuItem("Show in Explorer"))
			{
				char buffer[MAX_PATH];
				GetModuleFileName(NULL, buffer, MAX_PATH);
				std::string::size_type pos = std::string(buffer).find_last_of("\\/");
				std::string exeDir = std::string(buffer).substr(0, pos);

				std::string path = exeDir + "\\..\\..\\Engine\\" + texturePath;

				std::replace(path.begin(), path.end(), '/', '\\');

				std::string command = "/select," + path;
				ShellExecute(NULL, "open", "explorer.exe", command.c_str(), NULL, SW_SHOWDEFAULT);
			}

			if (ImGui::MenuItem("Open Image"))
			{
				char buffer[MAX_PATH];
				GetModuleFileName(NULL, buffer, MAX_PATH);
				std::string::size_type pos = std::string(buffer).find_last_of("\\/");
				std::string exeDir = std::string(buffer).substr(0, pos);

				std::string path = exeDir + "\\..\\..\\Engine\\" + texturePath;

				ShellExecute(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
			}
		}
	}
}

void ComponentMaterial::AddMaterial(int id, int width, int height, const char* path)
{
	if (gameObject->GetComponent(ComponentType::MESH) != nullptr)
	{
		if (gameObject->GetComponent(ComponentType::MATERIAL) == nullptr)
			gameObject->AddComponent(gameObject->material);	
		
		textureId = id;
		textureWidth = width;
		textureHeight = height;
		texturePath = path;
	}

	if (!gameObject->children.empty())
	{
		for (unsigned int i = 0; i < gameObject->children.size(); i++)
		{
			if (gameObject->children[i]->GetComponent(ComponentType::MATERIAL) == nullptr)
				gameObject->children[i]->AddComponent(gameObject->children[i]->material);

			gameObject->children[i]->material->textureId = id;
			gameObject->children[i]->material->textureWidth = width;
			gameObject->children[i]->material->textureHeight = height;
			gameObject->children[i]->material->texturePath = path;
		}
	}
}