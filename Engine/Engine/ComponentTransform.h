#pragma once

#include "Component.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

using namespace glm;

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* gameObject);
	virtual ~ComponentTransform();

	void Update() override;
	void OnEditor() override;

	void SetTransformMatrix(float3 position, quat rotation, float3 scale, ComponentTransform* parent);
	void UpdateTransform();

public:
	float4x4 localTransform;
	float4x4 globalTransform;

	float3 position;
	Quat rotation;
	float3 eulerRotation;
	float3 scale;

	bool constrainedProportions = false;
	float initialScale[3] = { 1.0f, 1.0f, 1.0f };
	bool updateTransform = false;
};

