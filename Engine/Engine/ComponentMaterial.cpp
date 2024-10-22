#include "ComponentMaterial.h"
#include "GameObject.h"

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
	if (ImGui::CollapsingHeader("Material"))
	{

	}
}

void ComponentMaterial::AddMaterial(int id)
{
	textureId = id;

	if (gameObject->mesh->mesh != nullptr)
		gameObject->mesh->mesh->textureID = textureId;

	if (!gameObject->children.empty())
	{
		for (unsigned int i = 0; i < gameObject->children.size(); i++)
		{
			gameObject->children[i]->mesh->mesh->textureID = textureId;
		}
	}
}