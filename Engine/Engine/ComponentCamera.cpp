#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "App.h"

ComponentCamera::ComponentCamera(GameObject* owner) : Component(owner, ComponentType::CAMERA)
{

}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::Update()
{
    if (gameObject && gameObject->transform)
    {
        // Update camera orientation based on transform
        glm::vec3 position = gameObject->transform->position;
        glm::vec3 rotation = glm::degrees(glm::eulerAngles(gameObject->transform->rotation));


        // Reset orientation vectors
        X = glm::vec3(1.0f, 0.0f, 0.0f);
        Y = glm::vec3(0.0f, 1.0f, 0.0f);
        Z = glm::vec3(0.0f, 0.0f, 1.0f);

        // Apply rotations
        glm::mat4 rotationMatrix = glm::eulerAngleXYZ(
            glm::radians(rotation.x),
            glm::radians(rotation.y),
            glm::radians(rotation.z)
        );

        X = glm::vec3(rotationMatrix * glm::vec4(X, 0.0f));
        Y = glm::vec3(rotationMatrix * glm::vec4(Y, 0.0f));
        Z = glm::vec3(rotationMatrix * glm::vec4(Z, 0.0f));

        CalculateViewMatrix();
        CalculateFrustumPlanes();
    }
}

glm::mat4 ComponentCamera::GetViewMatrix() const
{
    return viewMatrix;
}

glm::mat4 ComponentCamera::GetProjectionMatrix() const
{
    float aspectRatio = app->editor->sceneWindow->windowSize.x /
        app->editor->sceneWindow->windowSize.y;
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void ComponentCamera::LookAt(const glm::vec3& target)
{
    if (gameObject && gameObject->transform)
    {
        glm::vec3 position = gameObject->transform->position;
        Z = glm::normalize(position - target);
        X = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Z));
        Y = glm::cross(Z, X);

        // Calculate euler angles from look direction
        float pitch = glm::degrees(asin(-Z.y));
        float yaw = glm::degrees(atan2(Z.x, Z.z));

        gameObject->transform->rotation = glm::vec3(pitch, yaw, 0.0f);
        CalculateViewMatrix();
    }
}

void ComponentCamera::CalculateViewMatrix()
{
    if (gameObject && gameObject->transform)
    {
        glm::vec3 position = gameObject->transform->position;
        viewMatrix = glm::mat4(
            X.x, Y.x, Z.x, 0.0f,
            X.y, Y.y, Z.y, 0.0f,
            X.z, Y.z, Z.z, 0.0f,
            -glm::dot(X, position), -glm::dot(Y, position), -glm::dot(Z, position), 1.0f
        );
    }
}

void ComponentCamera::CalculateFrustumPlanes()
{
    glm::mat4 viewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix();

    // Extract frustum planes
    for (int i = 0; i < 6; ++i) {
        int row = i / 2;
        float sign = (i % 2 == 0) ? 1.0f : -1.0f;

        frustumPlanes[i].normal.x = viewProjectionMatrix[0][3] + sign * viewProjectionMatrix[0][row];
        frustumPlanes[i].normal.y = viewProjectionMatrix[1][3] + sign * viewProjectionMatrix[1][row];
        frustumPlanes[i].normal.z = viewProjectionMatrix[2][3] + sign * viewProjectionMatrix[2][row];
        frustumPlanes[i].distance = viewProjectionMatrix[3][3] + sign * viewProjectionMatrix[3][row];

        float length = glm::length(frustumPlanes[i].normal);
        frustumPlanes[i].normal /= length;
        frustumPlanes[i].distance /= length;
    }
}

bool ComponentCamera::IsBoxInsideFrustum(const AABB& box) const
{
    for (const auto& plane : frustumPlanes)
    {
        glm::vec3 p = box.minPoint;
        if (plane.normal.x >= 0) p.x = box.maxPoint.x;
        if (plane.normal.y >= 0) p.y = box.maxPoint.y;
        if (plane.normal.z >= 0) p.z = box.maxPoint.z;

        if (glm::dot(plane.normal, p) + plane.distance < 0)
            return false;
    }
    return true;
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

}
