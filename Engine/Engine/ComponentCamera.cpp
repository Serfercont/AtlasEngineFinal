// ComponentCamera.cpp
#include "ComponentCamera.h"
#include "GameObject.h"
#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"

ComponentCamera::ComponentCamera(GameObject* owner, ModuleCamera* moduleCamera)
    : Component(owner, ComponentType::CAMERA), moduleCamera(moduleCamera)
{
    if (moduleCamera) {
        pos = moduleCamera->GetPos();
        ref = moduleCamera->GetRef();
        fov = moduleCamera->fov;
        nearPlane = moduleCamera->nearPlane;
        farPlane = moduleCamera->farPlane;
    }
}

ComponentCamera::~ComponentCamera()
{}

void ComponentCamera::Update()
{
    if (moduleCamera) {
        moduleCamera->SetPos(pos);
        moduleCamera->SetRef(ref);
        moduleCamera->CalculateViewMatrix();
    }
}

void ComponentCamera::OnEditor()
{
    if (ImGui::CollapsingHeader("Camera"))
    {
  
        float fovDegrees = glm::degrees(fov);

        if (ImGui::SliderFloat("Field of View", &fovDegrees, MIN_FOV, MAX_FOV, "%.1f°"))
        {
            fov = glm::radians(fovDegrees);
            if (moduleCamera) moduleCamera->fov = fov;
        }
        if (ImGui::Button("Default (60°)"))
        {
            fov = glm::radians(60.0f);
            if (moduleCamera) moduleCamera->fov = fov;
        }
        ImGui::SameLine();
        if (ImGui::Button("Wide (90°)"))
        {
            fov = glm::radians(90.0f);
            if (moduleCamera) moduleCamera->fov = fov;
        }
        ImGui::SameLine();
        if (ImGui::Button("Narrow (45°)"))
        {
            fov = glm::radians(45.0f);
            if (moduleCamera) moduleCamera->fov = fov;
        }

        if (ImGui::DragFloat("Near Plane", &nearPlane, 0.1f, 0.001f, farPlane))
        {
            if (moduleCamera) moduleCamera->nearPlane = nearPlane;
        }

        if (ImGui::DragFloat("Far Plane", &farPlane, 1.0f, nearPlane, 10000.0f))
        {
            if (moduleCamera) moduleCamera->farPlane = farPlane;
        }

        ImGui::Separator();

     
        if (ImGui::Checkbox("Orthographic", &isOrthographic))
        {
            
        }

        ImGui::Separator();

        DrawCameraControls();

       
        const char* clearOptions[] = { "Skybox", "Solid Color", "Depth Only", "Don't Clear" };
        ImGui::Combo("Clear Flags", &selectedClearFlag, clearOptions, IM_ARRAYSIZE(clearOptions));

        
        ImGui::Checkbox("Cull Everything", &cullEverything);

        ImGui::Separator();

        DrawDebugInfo();
    }
}

void ComponentCamera::DrawCameraControls()
{
    if (ImGui::TreeNode("Camera Controls"))
    {
        if (ImGui::Button("Reset Camera"))
        {
            ResetCamera();
        }

       
        int viewWidth = moduleCamera ? moduleCamera->screenWidth : 0;
        int viewHeight = moduleCamera ? moduleCamera->screenHeight : 0;
        ImGui::Text("Viewport Size: %dx%d", viewWidth, viewHeight);

       
        float aspectRatio = viewWidth / (float)viewHeight;
        ImGui::Text("Aspect Ratio: %.2f", aspectRatio);

        ImGui::TreePop();
    }
}

void ComponentCamera::DrawDebugInfo()
{
    if (ImGui::TreeNode("Debug Info"))
    {
        ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
        ImGui::Text("Look At: (%.2f, %.2f, %.2f)", ref.x, ref.y, ref.z);

        if (ImGui::TreeNode("Matrices"))
        {
            glm::mat4 viewMatrix = GetViewMatrix();
            glm::mat4 projMatrix = GetProjectionMatrix();

            ImGui::Text("View Matrix:");
            for (int i = 0; i < 4; i++)
            {
                ImGui::Text("%.2f %.2f %.2f %.2f",
                    viewMatrix[i].x, viewMatrix[i].y,
                    viewMatrix[i].z, viewMatrix[i].w);
            }

            ImGui::Text("Projection Matrix:");
            for (int i = 0; i < 4; i++)
            {
                ImGui::Text("%.2f %.2f %.2f %.2f",
                    projMatrix[i].x, projMatrix[i].y,
                    projMatrix[i].z, projMatrix[i].w);
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

void ComponentCamera::ResetCamera()
{
    
    pos = glm::vec3(0.0f, 1.0f, -10.0f);
    ref = glm::vec3(0.0f, 0.0f, 0.0f);

    
    fov = glm::radians(60.0f); 
    nearPlane = 0.1f;          
    farPlane = 1000.0f;         
   
    isOrthographic = false;
    selectedClearFlag = 0;
    cullEverything = false;

 
    if (moduleCamera) {
        moduleCamera->SetPos(pos);
        moduleCamera->SetRef(ref);
        moduleCamera->LookAt(ref);
        moduleCamera->fov = fov;
        moduleCamera->nearPlane = nearPlane;
        moduleCamera->farPlane = farPlane;
    }
}

glm::mat4 ComponentCamera::GetViewMatrix() const
{
    return moduleCamera ? moduleCamera->GetViewMatrix() : glm::mat4(1.0f);
}

glm::mat4 ComponentCamera::GetProjectionMatrix() const
{
    return moduleCamera ? moduleCamera->GetProjectionMatrix() : glm::mat4(1.0f);
}

void ComponentCamera::LookAt(const glm::vec3& spot)
{
    if (moduleCamera) {
        moduleCamera->LookAt(spot);
        ref = spot;
    }
}