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

	void LookAt(const glm::vec3& spot);
	float* GetViewMatrix();

private:
	void HandleMovement(glm::vec3& newPos, float speed, float fastSpeed);
	void HandleZoom(float zoomSpeed);
	void HandleRotation();
	void RotateCamera(int dx, int dy);
	void CalculateViewMatrix();
	glm::vec3 RotateVector(glm::vec3 const& vector, float angle, glm::vec3 const& axis);

private:
	glm::vec3 X, Y, Z, Position, Reference;
	glm::mat4x4 ViewMatrix;
};