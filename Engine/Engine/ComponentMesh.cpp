#include "ComponentMesh.h"
#include "App.h"

#include <glm/gtc/type_ptr.hpp>

ComponentMesh::ComponentMesh(GameObject* gameObject) : Component(gameObject, ComponentType::MESH), mesh(nullptr)
{
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::Update()
{
	ComponentTransform* transform = gameObject->transform;

	if (transform != nullptr)
	{
		glPushMatrix();
		glMultMatrixf(glm::value_ptr(transform->globalTransform));
	}

	mesh->DrawMesh(gameObject->material->textureId, app->editor->drawTextures, app->editor->wireframe, app->editor->shadedWireframe);

	if (transform != nullptr) glPopMatrix();
}

void ComponentMesh::OnEditor()
{
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
	}
}