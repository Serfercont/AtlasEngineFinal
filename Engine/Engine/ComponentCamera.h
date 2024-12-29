// ComponentCamera.h
#pragma once
#include "Component.h"
#include "glm/glm.hpp"
#include "ModuleCamera.h"

class ComponentCamera : public Component {
public:
    ComponentCamera(GameObject* owner, ModuleCamera* moduleCamera);
    ~ComponentCamera();

    void Update() override;
    void OnEditor() override;

  
    void SetFOV(float newFOV) { fov = newFOV; }
    void SetNearPlane(float newNear) { nearPlane = newNear; }
    void SetFarPlane(float newFar) { farPlane = newFar; }

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    void LookAt(const glm::vec3& spot);

private:
    ModuleCamera* moduleCamera;
    glm::vec3 pos;
    glm::vec3 ref;
    float fov;
    float nearPlane;
    float farPlane;

    
    bool isOrthographic = false;
    int selectedClearFlag = 0;
    bool cullEverything = false;

    const float MIN_FOV = 20.0f;  
    const float MAX_FOV = 120.0f; 
    const float FOV_SPEED = 0.5f;
    void DrawCameraControls();
    void DrawDebugInfo();
    void ResetCamera();
};