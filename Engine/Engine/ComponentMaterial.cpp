#include "ComponentMaterial.h"

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