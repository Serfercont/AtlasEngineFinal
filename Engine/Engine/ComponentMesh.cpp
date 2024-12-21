#include "ComponentMesh.h"
#include "App.h"
#include "AABB.h"
#include "OBB.h"

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
    ComponentMaterial* material = gameObject->material;

    if (transform != nullptr)
    {
        const AABB globalAABB = mesh->CalculateAABB(transform->globalTransform);

        if (app->camera->IsBoxInsideFrustum(globalAABB))
        {
            glPushMatrix();
            glMultMatrixf(glm::value_ptr(transform->globalTransform));
           
            mesh->DrawMesh(
                material->textureId,
                app->editor->preferencesWindow->drawTextures,
                app->editor->preferencesWindow->wireframe,
                app->editor->preferencesWindow->shadedWireframe
            );

            if (showVertexNormals || showFaceNormals)
            {
                mesh->DrawNormals(
                    showVertexNormals,
                    showFaceNormals,
                    app->editor->preferencesWindow->vertexNormalLength,
                    app->editor->preferencesWindow->faceNormalLength,
                    app->editor->preferencesWindow->vertexNormalColor,
                    app->editor->preferencesWindow->faceNormalColor
                );
            }
            glPopMatrix();

            if (app->editor->selectedGameObject == gameObject)
            {
                if (showAABB)
                {
                    mesh->RenderAABB(transform->globalTransform);
                }
                if (showOBB)
                {
                    mesh->RenderOBB(transform->globalTransform);
                }
            }
        }
    }
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

		ImGui::Spacing();

		ImGui::Checkbox("Show AABB", &showAABB);
		ImGui::Checkbox("Show OBB", &showOBB);
	}
}

