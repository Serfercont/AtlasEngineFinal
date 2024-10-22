#pragma once

#include "Module.h"
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

using namespace glm;

class ModuleCamera : public Module
{
public:
	ModuleCamera(App* app);
	~ModuleCamera();

	bool Start();
	bool Update(float dt);
	void FrameSelected();
	bool CleanUp();

	void Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference = false);
	void LookAt(const vec3& Spot);
	void Move(const vec3& Movement);
	float* GetViewMatrix();

private:
	void CalculateViewMatrix();
	vec3 rotateVector(vec3 const& vector, float angle, vec3 const& axis);
public:
	vec3 X, Y, Z, Position, Reference;

private:
	mat4x4 ViewMatrix, ViewMatrixInverse;
};