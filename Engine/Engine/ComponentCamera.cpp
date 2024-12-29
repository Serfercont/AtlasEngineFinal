//// ComponentCamera.cpp
//#include "ComponentCamera.h"
//#include "GameObject.h"
//#include "imgui.h"
//#include "glm/gtc/matrix_transform.hpp"
//
//ComponentCamera::ComponentCamera(GameObject* owner, ModuleCamera* moduleCamera)
//    : Component(owner, ComponentType::CAMERA), moduleCamera(moduleCamera)
//{
//    if (moduleCamera) {
//        pos = moduleCamera->GetPos();
//        ref = moduleCamera->GetRef();
//        fov = moduleCamera->fov;
//        nearPlane = moduleCamera->nearPlane;
//        farPlane = moduleCamera->farPlane;
//    }
//}
//
//ComponentCamera::~ComponentCamera()
//{}
//
//void ComponentCamera::Update()
//{
//    if (moduleCamera) {
//        moduleCamera->SetPos(pos);
//        moduleCamera->SetRef(ref);
//        moduleCamera->CalculateViewMatrix();
//    }
//}
//
//void ComponentCamera::OnEditor()
//{
//    if (ImGui::CollapsingHeader("Camera"))
//    {
//  
//        float fovDegrees = glm::degrees(fov);
//
//        if (ImGui::SliderFloat("Field of View", &fovDegrees, MIN_FOV, MAX_FOV, "%.1f°"))
//        {
//            fov = glm::radians(fovDegrees);
//            if (moduleCamera) moduleCamera->fov = fov;
//        }
//        if (ImGui::Button("Default (60°)"))
//        {
//            fov = glm::radians(60.0f);
//            if (moduleCamera) moduleCamera->fov = fov;
//        }
//        ImGui::SameLine();
//        if (ImGui::Button("Wide (90°)"))
//        {
//            fov = glm::radians(90.0f);
//            if (moduleCamera) moduleCamera->fov = fov;
//        }
//        ImGui::SameLine();
//        if (ImGui::Button("Narrow (45°)"))
//        {
//            fov = glm::radians(45.0f);
//            if (moduleCamera) moduleCamera->fov = fov;
//        }
//
//        if (ImGui::DragFloat("Near Plane", &nearPlane, 0.1f, 0.001f, farPlane))
//        {
//            if (moduleCamera) moduleCamera->nearPlane = nearPlane;
//        }
//
//        if (ImGui::DragFloat("Far Plane", &farPlane, 1.0f, nearPlane, 10000.0f))
//        {
//            if (moduleCamera) moduleCamera->farPlane = farPlane;
//        }
//
//        ImGui::Separator();
//
//     
//        if (ImGui::Checkbox("Orthographic", &isOrthographic))
//        {
//            
//        }
//
//        ImGui::Separator();
//
//        DrawCameraControls();
//
//       
//        const char* clearOptions[] = { "Skybox", "Solid Color", "Depth Only", "Don't Clear" };
//        ImGui::Combo("Clear Flags", &selectedClearFlag, clearOptions, IM_ARRAYSIZE(clearOptions));
//
//        
//        ImGui::Checkbox("Cull Everything", &cullEverything);
//
//        ImGui::Separator();
//
//        DrawDebugInfo();
//    }
//}
//
//void ComponentCamera::DrawCameraControls()
//{
//    if (ImGui::TreeNode("Camera Controls"))
//    {
//        if (ImGui::Button("Reset Camera"))
//        {
//            ResetCamera();
//        }
//
//       
//        int viewWidth = moduleCamera ? moduleCamera->screenWidth : 0;
//        int viewHeight = moduleCamera ? moduleCamera->screenHeight : 0;
//        ImGui::Text("Viewport Size: %dx%d", viewWidth, viewHeight);
//
//       
//        float aspectRatio = viewWidth / (float)viewHeight;
//        ImGui::Text("Aspect Ratio: %.2f", aspectRatio);
//
//        ImGui::TreePop();
//    }
//}
//
//void ComponentCamera::DrawDebugInfo()
//{
//    if (ImGui::TreeNode("Debug Info"))
//    {
//        ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
//        ImGui::Text("Look At: (%.2f, %.2f, %.2f)", ref.x, ref.y, ref.z);
//
//        if (ImGui::TreeNode("Matrices"))
//        {
//            glm::mat4 viewMatrix = GetViewMatrix();
//            glm::mat4 projMatrix = GetProjectionMatrix();
//
//            ImGui::Text("View Matrix:");
//            for (int i = 0; i < 4; i++)
//            {
//                ImGui::Text("%.2f %.2f %.2f %.2f",
//                    viewMatrix[i].x, viewMatrix[i].y,
//                    viewMatrix[i].z, viewMatrix[i].w);
//            }
//
//            ImGui::Text("Projection Matrix:");
//            for (int i = 0; i < 4; i++)
//            {
//                ImGui::Text("%.2f %.2f %.2f %.2f",
//                    projMatrix[i].x, projMatrix[i].y,
//                    projMatrix[i].z, projMatrix[i].w);
//            }
//            ImGui::TreePop();
//        }
//        ImGui::TreePop();
//    }
//}
//
//void ComponentCamera::ResetCamera()
//{
//    
//    pos = glm::vec3(0.0f, 1.0f, -10.0f);
//    ref = glm::vec3(0.0f, 0.0f, 0.0f);
//
//    
//    fov = glm::radians(60.0f); 
//    nearPlane = 0.1f;          
//    farPlane = 1000.0f;         
//   
//    isOrthographic = false;
//    selectedClearFlag = 0;
//    cullEverything = false;
//
// 
//    if (moduleCamera) {
//        moduleCamera->SetPos(pos);
//        moduleCamera->SetRef(ref);
//        moduleCamera->LookAt(ref);
//        moduleCamera->fov = fov;
//        moduleCamera->nearPlane = nearPlane;
//        moduleCamera->farPlane = farPlane;
//    }
//}
//
//glm::mat4 ComponentCamera::GetViewMatrix() const
//{
//    return moduleCamera ? moduleCamera->GetViewMatrix() : glm::mat4(1.0f);
//}
//
//glm::mat4 ComponentCamera::GetProjectionMatrix() const
//{
//    return moduleCamera ? moduleCamera->GetProjectionMatrix() : glm::mat4(1.0f);
//}
//
//void ComponentCamera::LookAt(const glm::vec3& spot)
//{
//    if (moduleCamera) {
//        moduleCamera->LookAt(spot);
//        ref = spot;
//    }
//}

#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "App.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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

void ComponentCamera::DrawGizmo()
{
    if (!gameObject || !gameObject->transform) return;

    // Guardar el estado actual de OpenGL
    glPushMatrix();
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LINE_BIT);

    // Deshabilitar el depth test para que el gizmo siempre sea visible
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    // Configurar el color y grosor de línea
    if (isMainCamera)
        glColor3f(1.0f, 0.5f, 0.0f); // Naranja para la cámara principal
    else
        glColor3f(0.2f, 0.8f, 1.0f); // Azul claro para las demás cámaras

    glLineWidth(2.0f);

    // Aplicar la transformación del objeto
    glm::mat4 transform = gameObject->transform->globalTransform;
    glMultMatrixf(glm::value_ptr(transform));

    // Dibujar el icono de la cámara
    DrawCameraIcon();

    // Dibujar el frustum
    DrawFrustum();

    // Restaurar el estado de OpenGL
    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glPopAttrib();
    glPopMatrix();
}

void ComponentCamera::DrawCameraIcon()
{
    float s = gizmoSize;

    // Dibujar el cuerpo de la cámara (un cubo)
    glBegin(GL_LINES);
    // Frente
    glVertex3f(-s, -s, -s);
    glVertex3f(s, -s, -s);
    glVertex3f(s, -s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(s, s, -s);
    glVertex3f(-s, s, -s);
    glVertex3f(-s, s, -s);
    glVertex3f(-s, -s, -s);

    // Atrás
    glVertex3f(-s, -s, s);
    glVertex3f(s, -s, s);
    glVertex3f(s, -s, s);
    glVertex3f(s, s, s);
    glVertex3f(s, s, s);
    glVertex3f(-s, s, s);
    glVertex3f(-s, s, s);
    glVertex3f(-s, -s, s);

    // Conectores
    glVertex3f(-s, -s, -s);
    glVertex3f(-s, -s, s);
    glVertex3f(s, -s, -s);
    glVertex3f(s, -s, s);
    glVertex3f(s, s, -s);
    glVertex3f(s, s, s);
    glVertex3f(-s, s, -s);
    glVertex3f(-s, s, s);
    glEnd();

    // Dibujar el "lente" de la cámara
    glBegin(GL_LINES);
    float front = -s * 1.5f; // El lente sobresale del cubo
    glVertex3f(-s * 0.5f, -s * 0.5f, front);
    glVertex3f(s * 0.5f, -s * 0.5f, front);
    glVertex3f(s * 0.5f, -s * 0.5f, front);
    glVertex3f(s * 0.5f, s * 0.5f, front);
    glVertex3f(s * 0.5f, s * 0.5f, front);
    glVertex3f(-s * 0.5f, s * 0.5f, front);
    glVertex3f(-s * 0.5f, s * 0.5f, front);
    glVertex3f(-s * 0.5f, -s * 0.5f, front);

    // Conectar el lente con el cuerpo
    glVertex3f(-s * 0.5f, -s * 0.5f, front);
    glVertex3f(-s, -s, -s);
    glVertex3f(s * 0.5f, -s * 0.5f, front);
    glVertex3f(s, -s, -s);
    glVertex3f(s * 0.5f, s * 0.5f, front);
    glVertex3f(s, s, -s);
    glVertex3f(-s * 0.5f, s * 0.5f, front);
    glVertex3f(-s, s, -s);
    glEnd();
}

void ComponentCamera::DrawFrustum()
{
    // Calcular los puntos del frustum cerca
    float nearHeight = 2 * nearPlane * tanf(glm::radians(fov) * 0.5f);
    float nearWidth = nearHeight * aspectRatio;
    float nearHalfHeight = nearHeight * 0.5f;
    float nearHalfWidth = nearWidth * 0.5f;

    // Calcular los puntos del frustum lejos
    float farHeight = 2 * farPlane * tanf(glm::radians(fov) * 0.5f);
    float farWidth = farHeight * aspectRatio;
    float farHalfHeight = farHeight * 0.5f;
    float farHalfWidth = farWidth * 0.5f;

    // Dibujar el frustum
    glBegin(GL_LINES);
    // Near plane
    glVertex3f(-nearHalfWidth, -nearHalfHeight, -nearPlane);
    glVertex3f(nearHalfWidth, -nearHalfHeight, -nearPlane);
    glVertex3f(nearHalfWidth, -nearHalfHeight, -nearPlane);
    glVertex3f(nearHalfWidth, nearHalfHeight, -nearPlane);
    glVertex3f(nearHalfWidth, nearHalfHeight, -nearPlane);
    glVertex3f(-nearHalfWidth, nearHalfHeight, -nearPlane);
    glVertex3f(-nearHalfWidth, nearHalfHeight, -nearPlane);
    glVertex3f(-nearHalfWidth, -nearHalfHeight, -nearPlane);

    // Far plane
    glVertex3f(-farHalfWidth, -farHalfHeight, -farPlane);
    glVertex3f(farHalfWidth, -farHalfHeight, -farPlane);
    glVertex3f(farHalfWidth, -farHalfHeight, -farPlane);
    glVertex3f(farHalfWidth, farHalfHeight, -farPlane);
    glVertex3f(farHalfWidth, farHalfHeight, -farPlane);
    glVertex3f(-farHalfWidth, farHalfHeight, -farPlane);
    glVertex3f(-farHalfWidth, farHalfHeight, -farPlane);
    glVertex3f(-farHalfWidth, -farHalfHeight, -farPlane);

    // Conectores
    glVertex3f(-nearHalfWidth, -nearHalfHeight, -nearPlane);
    glVertex3f(-farHalfWidth, -farHalfHeight, -farPlane);
    glVertex3f(nearHalfWidth, -nearHalfHeight, -nearPlane);
    glVertex3f(farHalfWidth, -farHalfHeight, -farPlane);
    glVertex3f(nearHalfWidth, nearHalfHeight, -nearPlane);
    glVertex3f(farHalfWidth, farHalfHeight, -farPlane);
    glVertex3f(-nearHalfWidth, nearHalfHeight, -nearPlane);
    glVertex3f(-farHalfWidth, farHalfHeight, -farPlane);
    glEnd();
}