#include "ComponentMaterial.h"
#include "GameObject.h"

#include <windows.h>
#include <shellapi.h>
#include <algorithm> 

ComponentMaterial::ComponentMaterial(GameObject* gameObject) : Component(gameObject, ComponentType::MATERIAL), materialTexture(nullptr)
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
		if (materialTexture->textureId != -1)
		{
			ImGui::Text("Path: %s", materialTexture->texturePath);
			ImGui::Text("Texture Size: %i x %i", materialTexture->textureWidth, materialTexture->textureHeight);
			ImGui::Image((int*)materialTexture->textureId, ImVec2(200, 200));

			if (ImGui::MenuItem("Show in Explorer"))
			{
				char buffer[MAX_PATH];
				GetModuleFileName(NULL, buffer, MAX_PATH);
				std::string::size_type pos = std::string(buffer).find_last_of("\\/");
				std::string exeDir = std::string(buffer).substr(0, pos);

				std::string path = exeDir + "\\..\\..\\Engine\\" + materialTexture->texturePath;

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

				std::string path = exeDir + "\\..\\..\\Engine\\" + materialTexture->texturePath;

				ShellExecute(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
			}
		}
	}
}

void ComponentMaterial::AddTexture(Texture* texture)
{
	if (gameObject->GetComponent(ComponentType::MESH) != nullptr)
	{
		if (gameObject->GetComponent(ComponentType::MATERIAL) == nullptr)
			gameObject->AddComponent(gameObject->material);	
		
		materialTexture = texture;
	}

	if (!gameObject->children.empty())
	{
		for (unsigned int i = 0; i < gameObject->children.size(); i++)
		{
			if (gameObject->children[i]->GetComponent(ComponentType::MATERIAL) == nullptr)
				gameObject->children[i]->AddComponent(gameObject->children[i]->material);

			gameObject->children[i]->material->materialTexture = texture;
		}
	}
}