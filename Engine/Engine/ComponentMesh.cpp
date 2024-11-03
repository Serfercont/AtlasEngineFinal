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

	ComponentMaterial* material = gameObject->material; 

	mesh->DrawMesh(
		material->textureId, 
		app->editor->drawTextures, 
		app->editor->wireframe, 
		app->editor->shadedWireframe
	);

	if (showVertexNormals || showFaceNormals)
	{
		mesh->DrawNormals(
			showVertexNormals, 
			showFaceNormals, 
			app->editor->vertexNormalLength,
			app->editor->faceNormalLength,
			app->editor->vertexNormalColor, 
			app->editor->faceNormalColor
		);
	}

	if (transform != nullptr) glPopMatrix();
}

void ComponentMesh::OnEditor()
{
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Vertices: %d", mesh->verticesCount);
		ImGui::Text("Indices: %d", mesh->indicesCount);
		ImGui::Text("Normals: %d", mesh->normalsCount);
		ImGui::Text("Texture Coords: %d", mesh->texCoordsCount);

		ImGui::Spacing();

		ImGui::Checkbox("Vertex Normals", &showVertexNormals);
		ImGui::Checkbox("Face Normals", &showFaceNormals);
	}
}