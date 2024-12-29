#pragma once
#include "Component.h"
#include "AABB.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ComponentCamera : public Component {
public:
    ComponentCamera(GameObject* owner);
    ~ComponentCamera();

    void Update() override;

    // Camera properties
    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    bool isMainCamera = false;

    // Camera matrices
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    // Camera vectors
    glm::vec3 GetForward() const { return -Z; }
    glm::vec3 GetRight() const { return X; }
    glm::vec3 GetUp() const { return Y; }

    // Utility functions
    void LookAt(const glm::vec3& target);
    void CalculateViewMatrix();
    void CalculateFrustumPlanes();
    bool IsBoxInsideFrustum(const AABB& box) const;

	void OnEditor() override;

private:
    // Camera orientation vectors
    glm::vec3 X = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 Y = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Z = glm::vec3(0.0f, 0.0f, 1.0f);

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    struct Plane {
        glm::vec3 normal = glm::vec3(0.0f);
        float distance = 0.0f;
    } frustumPlanes[6];
};