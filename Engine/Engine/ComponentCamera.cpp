#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "App.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

ComponentCamera::ComponentCamera(GameObject* owner) : Component(owner, ComponentType::CAMERA)
{
    screenWidth = app->window->width;
    screenHeight = app->window->height;

    CalculateViewMatrix();

    X = glm::vec3(1.0f, 0.0f, 0.0f);
    Y = glm::vec3(0.0f, 1.0f, 0.0f);
    Z = glm::vec3(0.0f, 0.0f, 1.0f);

    position = glm::vec3(0.0f, 5.0f, 5.0f);
    reference = glm::vec3(0.0f, 0.0f, 0.0f);
}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::Update()
{
 
    CalculateViewMatrix();
    CalculateFrustumPlanes();

    meshCount = 0;
    vertexCount = 0;
    triangleCount = 0;

    if (gameObject)
    {
        position = gameObject->transform->position;

        X = glm::vec3(gameObject->transform->localTransform[0]);
        Y = glm::vec3(gameObject->transform->localTransform[1]);
        Z = glm::vec3(gameObject->transform->localTransform[2]);
    }

	if (CalculateFrust)
	{
		CalculateFrustumPlanes();
		CalculateFrust = false;
	}
   
}

glm::mat4 ComponentCamera::GetViewMatrix() const
{
    return viewMatrix;
}

glm::mat4 ComponentCamera::GetProjectionMatrix() const
{
    float aspectRatio = app->editor->sceneWindow->windowSize.x / app->editor->sceneWindow->windowSize.y;
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void ComponentCamera::CalculateViewMatrix()
{
        viewMatrix = glm::mat4(
            X.x, Y.x, Z.x, 0.0f,
            X.y, Y.y, Z.y, 0.0f,
            X.z, Y.z, Z.z, 0.0f,
            -glm::dot(X, position), -glm::dot(Y, position), -glm::dot(Z, position), 1.0f
        );
}

void ComponentCamera::CalculateFrustumPlanes()
{
    glm::mat4 viewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix();

    const int axisIndices[6] = { 0, 0, 1, 1, 2, 2 };
    const float planeSigns[6] = { 1, -1, 1, -1, 1, -1 };

    for (int i = 0; i < 6; ++i) {
        int axis = axisIndices[i];
        float sign = planeSigns[i];

        frustumPlanes[i].normal.x = viewProjectionMatrix[0][3] + sign * viewProjectionMatrix[0][axis];
        frustumPlanes[i].normal.y = viewProjectionMatrix[1][3] + sign * viewProjectionMatrix[1][axis];
        frustumPlanes[i].normal.z = viewProjectionMatrix[2][3] + sign * viewProjectionMatrix[2][axis];
        frustumPlanes[i].distance = viewProjectionMatrix[3][3] + sign * viewProjectionMatrix[3][axis];

        float length = glm::length(frustumPlanes[i].normal);
        frustumPlanes[i].normal /= length;
        frustumPlanes[i].distance /= length;
    }
}

bool ComponentCamera::IsBoxInsideFrustum(const AABB& box) const
{
    for (int i = 0; i < 6; ++i)
    {
        const Plane& currentPlane = frustumPlanes[i];
        bool isAnyCornerInside = false;

        glm::vec3 corners[8] = {
            { box.minPoint.x, box.minPoint.y, box.minPoint.z },
            { box.maxPoint.x, box.minPoint.y, box.minPoint.z },
            { box.minPoint.x, box.maxPoint.y, box.minPoint.z },
            { box.maxPoint.x, box.maxPoint.y, box.minPoint.z },
            { box.minPoint.x, box.minPoint.y, box.maxPoint.z },
            { box.maxPoint.x, box.minPoint.y, box.maxPoint.z },
            { box.minPoint.x, box.maxPoint.y, box.maxPoint.z },
            { box.maxPoint.x, box.maxPoint.y, box.maxPoint.z }
        };

        for (int cornerIndex = 0; cornerIndex < 8; ++cornerIndex)
        {
            float distanceToPlane = glm::dot(currentPlane.normal, corners[cornerIndex]) + currentPlane.distance;
            if (distanceToPlane >= 0)
            {
                isAnyCornerInside = true;
                break;
            }
        }

        if (!isAnyCornerInside)
        {
            return false;
        }
    }
}

void ComponentCamera::OnEditor()
{
    if (ImGui::Checkbox("Main Camera", &isMainCamera)) {
        // Si esta cámara se marca como principal, desmarcar las demás
        if (isMainCamera) {
            for (auto* go : app->scene->GetGameObjects()) {
                if (go->camera && go->camera != this) {
                    go->camera->isMainCamera = false;
                }
            }
        }

    }
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Fov ");
        ImGui::SameLine();
        ImGui::SliderFloat("##Fov", &app->scene->MainGameCamera->fov, 4.0f, 120.0f);

        ImGui::Text("Near");
        ImGui::SameLine();
        ImGui::InputFloat("##Near", &app->scene->MainGameCamera->nearPlane);

        ImGui::Text("Far ");
        ImGui::SameLine();
        ImGui::InputFloat("##Far", &app->scene->MainGameCamera->farPlane);
    }

}


void ComponentCamera::DrawFrustum() {
    glm::mat4 vpMatrix = GetProjectionMatrix() * GetViewMatrix();
    glm::mat4 invVPMatrix = glm::inverse(vpMatrix);

    glm::vec4 corners[8] = {
        {-1, -1, -1, 1}, {1, -1, -1, 1},
        {1,  1, -1, 1}, {-1,  1, -1, 1},
        {-1, -1,  1, 1}, {1, -1,  1, 1},
        {1,  1,  1, 1}, {-1,  1,  1, 1}
    };

    for (auto& corner : corners) {
        corner = invVPMatrix * corner;
        corner /= corner.w;
    }

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);

    glBegin(GL_LINES);
    for (int i = 0; i < 4; ++i) {
        glVertex3fv(&corners[i].x); glVertex3fv(&corners[(i + 1) % 4].x);
        glVertex3fv(&corners[i + 4].x); glVertex3fv(&corners[((i + 1) % 4) + 4].x);
        glVertex3fv(&corners[i].x); glVertex3fv(&corners[i + 4].x);
    }
    glEnd();

    glPopAttrib();
}
