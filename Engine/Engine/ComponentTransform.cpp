#include "ComponentTransform.h"
#include "GameObject.h"

ComponentTransform::ComponentTransform(GameObject* gameObject) : Component(gameObject, ComponentType::TRANSFORM)
{
    localTransform = float4x4(1.0f);
    globalTransform = float4x4(1.0f);

    Decompose(globalTransform, position, rotation, scale);

    eulerRotation = eulerAngles(rotation);

}

ComponentTransform::~ComponentTransform()
{

}

void ComponentTransform::Update()
{

}

void ComponentTransform::OnEditor()
{
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const char* labels[] = { "X", "Y", "Z" };

        // Position
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Position   ");
        ImGui::PushItemWidth(40);

        for (int i = 0; i < 3; ++i) {
            ImGui::SameLine();
            ImGui::AlignTextToFramePadding();
            ImGui::Text(labels[i]);
            ImGui::SameLine();
            float floatValue = position[i];
            std::string formattedValue = std::to_string(floatValue);
            formattedValue.erase(formattedValue.find_last_not_of('0') + 1, std::string::npos);
            formattedValue.erase(formattedValue.find_last_not_of('.') + 1, std::string::npos);
            if (ImGui::DragFloat(std::string("##position_").append(labels[i]).c_str(), &position[i], 0.1f, 0.0f, 0.0f, formattedValue.c_str()))
            {
                updateTransform = true;
            }
        }
        ImGui::PopItemWidth();

        // Rotation
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Rotation   ");
        ImGui::PushItemWidth(40);

        for (int i = 0; i < 3; ++i) {
            ImGui::SameLine();
            ImGui::Text(labels[i]);
            ImGui::SameLine();

            float eulerAngle = eulerRotation[i];
            if (eulerAngle == -0.0f) {
                eulerAngle = 0.0f;
            }

            std::string formattedValue = std::to_string(eulerAngle);
            formattedValue.erase(formattedValue.find_last_not_of('0') + 1, std::string::npos);
            formattedValue.erase(formattedValue.find_last_not_of('.') + 1, std::string::npos);
            if (ImGui::DragFloat(std::string("##rotation_").append(labels[i]).c_str(), &eulerRotation[i], 0.1f, 0.0f, 0.0f, formattedValue.c_str()))
            {
                updateTransform = true;
            }
        }
        ImGui::PopItemWidth();

        // Scale
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Scale  ");
        ImGui::SameLine();

        if (ImGui::Button("##Constrained", ImVec2(20, 20))) {
            constrainedProportions = !constrainedProportions;
            if (constrainedProportions) {
                for (int i = 0; i < 3; ++i) {
                    initialScale[i] = scale[i];
                }
            }
        }

        static float previousScales[3];
        previousScales[0] = scale.x;
        previousScales[1] = scale.y;
        previousScales[2] = scale.z;

        ImGui::PushItemWidth(40);

        bool scaleChanged = false;
        for (int i = 0; i < 3; ++i) {
            ImGui::SameLine();
            ImGui::AlignTextToFramePadding();
            ImGui::Text(labels[i]);
            ImGui::SameLine();
            float floatValue = scale[i];
            std::string formattedValue = std::to_string(floatValue);
            formattedValue.erase(formattedValue.find_last_not_of('0') + 1, std::string::npos);
            formattedValue.erase(formattedValue.find_last_not_of('.') + 1, std::string::npos);
            if (ImGui::DragFloat(std::string("##scale_").append(labels[i]).c_str(), &scale[i], 0.1f, 0.0f, 0.0f, formattedValue.c_str()))
            {
                updateTransform = true;
            }
            scaleChanged |= ImGui::IsItemEdited();
        }

        if (scaleChanged && constrainedProportions) {
            for (int i = 0; i < 3; ++i) {
                if (previousScales[i] != scale[i]) {
                    float ratio = scale[i] / previousScales[i];
                    for (int j = 0; j < 3; ++j) {
                        if (j != i) {
                            scale[j] = previousScales[j] * ratio;
                        }
                    }
                    break;
                }
            }
        }

        ImGui::PopItemWidth();

        // Reset
        if (ImGui::Button("Reset"))
        {
            position = float3(0.f);
            eulerRotation = float3(0.f);
            scale = float3(1.f);
            updateTransform = true;
        }
    }

    if (updateTransform) UpdateTransform();
}

void ComponentTransform::SetTransformMatrix(float3 position, quat rotation, float3 scale, ComponentTransform* parent)
{
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;

    eulerRotation = degrees(glm::eulerAngles(rotation));

    localTransform = float4x4(1.0f);
    localTransform = glm::translate(localTransform, position);
    localTransform *= glm::mat4_cast(rotation);
    localTransform = glm::scale(localTransform, scale);

    if (parent != nullptr)
    {
        globalTransform = parent->globalTransform * localTransform;
    }
}

void ComponentTransform::UpdateTransform()
{
    rotation = quat(vec3(radians(eulerRotation.x), radians(eulerRotation.y), radians(eulerRotation.z)));

    localTransform = float4x4(1.0f);
    localTransform = glm::translate(localTransform, position);
    localTransform *= glm::mat4_cast(rotation);
    localTransform = glm::scale(localTransform, scale);

    if (gameObject->parent != nullptr)
    {
        ComponentTransform* parentTransform = gameObject->parent->transform;
        globalTransform = parentTransform->globalTransform * localTransform;
    }
    else
    {
        globalTransform = localTransform;
    }

    for (auto child : gameObject->children)
    {
        child->transform->UpdateTransform();
    }

    updateTransform = false;
}

bool ComponentTransform::Decompose(const float4x4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale)
{
    float4x4 localMatrix(transform);

    translation = glm::vec3(localMatrix[3]);

    glm::vec3 row[3];

    for (int i = 0; i < 3; i++)
    {
        row[i] = glm::vec3(localMatrix[i]);
    }

    scale.x = glm::length(row[0]);
    scale.y = glm::length(row[1]);
    scale.z = glm::length(row[2]);

    if (scale.x != 0) row[0] /= scale.x;
    if (scale.y != 0) row[1] /= scale.y;
    if (scale.z != 0) row[2] /= scale.z;

    glm::mat3 rotationMatrix(row[0], row[1], row[2]);

    rotation = glm::quat_cast(rotationMatrix);

    return true;
}