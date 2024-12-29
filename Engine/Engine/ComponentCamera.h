//// ComponentCamera.h
//#pragma once
//#include "Component.h"
//#include "glm/glm.hpp"
//#include "ModuleCamera.h"
//
//class ComponentCamera : public Component {
//public:
//    ComponentCamera(GameObject* owner, ModuleCamera* moduleCamera);
//    ~ComponentCamera();
//
//    void Update() override;
//    void OnEditor() override;
//
//  
//    void SetFOV(float newFOV) { fov = newFOV; }
//    void SetNearPlane(float newNear) { nearPlane = newNear; }
//    void SetFarPlane(float newFar) { farPlane = newFar; }
//
//    glm::mat4 GetViewMatrix() const;
//    glm::mat4 GetProjectionMatrix() const;
//    void LookAt(const glm::vec3& spot);
//
//private:
//    ModuleCamera* moduleCamera;
//    glm::vec3 pos;
//    glm::vec3 ref;
//    float fov;
//    float nearPlane;
//    float farPlane;
//
//    
//    bool isOrthographic = false;
//    int selectedClearFlag = 0;
//    bool cullEverything = false;
//
//    const float MIN_FOV = 20.0f;  
//    const float MAX_FOV = 120.0f; 
//    const float FOV_SPEED = 0.5f;
//    void DrawCameraControls();
//    void DrawDebugInfo();
//    void ResetCamera();
//};

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
	float aspectRatio = 1.0f;
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
    void DrawGizmo();

    void OnEditor() override;

private:
    void DrawFrustum();
    void DrawCameraIcon();
    float gizmoSize = 0.5f; // Tamaño del icono de la cámara

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