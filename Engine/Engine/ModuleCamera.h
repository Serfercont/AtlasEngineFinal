#pragma once

#include "Module.h"
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

class ModuleCamera : public Module
{
public:
	ModuleCamera(App* app);
	~ModuleCamera();

	bool Start();
	bool Update(float dt);
	void FrameSelected();
	bool CleanUp();

	void Look(const glm::vec3& Position, const glm::vec3& Reference, bool RotateAroundReference = false);
	void LookAt(const glm::vec3& Spot);
	void Move(const glm::vec3& Movement);
	float* GetViewMatrix();

private:
	void CalculateViewMatrix();
	glm::vec3 rotateVector(glm::vec3 const& vector, float angle, glm::vec3 const& axis);
public:
	glm::vec3 X, Y, Z, Position, Reference;

private:
	glm::mat4x4 ViewMatrix, ViewMatrixInverse;
};