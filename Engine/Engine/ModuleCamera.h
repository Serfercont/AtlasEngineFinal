#pragma once

#include "Module.h"
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include "Mesh.h"
#include "ModuleInput.h"
#include "AABB.h"


struct Plane
{
	glm::vec3 normal;
	float distance;
};



class ModuleCamera : public Module
{
public:
	const glm::vec3& GetPos() const { return pos; }
	void SetPos(const glm::vec3& newPos) { pos = newPos; }
	const glm::vec3& GetRef() const { return ref; }
	void SetRef(const glm::vec3& newRef) { ref = newRef; }

	ModuleCamera(App* app);
	~ModuleCamera();

	bool Start();
	bool Update(float dt);
	void HandleInput();
	void FrameSelected();
	bool CleanUp();
	void LookAt(const glm::vec3& spot);
	const glm::mat4& GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;
	bool IsBoxInsideFrustum(const AABB& box) const;

	void DrawFrustum();

	void OnWindowResized(int newWidth, int newHeight);
	glm::vec3 GetPosition() const { return pos; }


	void CalculateViewMatrix();
	void UpdateViewMatrix() { CalculateViewMatrix(); }
private:
	void HandleMovement(glm::vec3& newPos, float speed, float fastSpeed);
	void HandleZoom(float zoomSpeed);
	void HandleRotation();
	void RotateCamera(int dx, int dy);

	glm::vec3 RotateVector(glm::vec3 const& vector, float angle, glm::vec3 const& axis);

	void SetCursor(CursorType cursorType);

	void CalculateFrustumPlanes();

public:
	float fov = 60.0f;
	float nearPlane = 0.125f;
	float farPlane = 512.0f;
	int screenWidth, screenHeight;

private:
	glm::vec3 X, Y, Z;
	glm::vec3 pos, ref;
	glm::mat4 viewMatrix;

	bool isZooming = false;
	bool isOrbiting = false;
	bool isFreeLook = false;
	bool isDragging = false;
	bool isDefaultCursor = true;
	ModuleCamera* moduleCamera = nullptr;

	Plane frustumPlanes[6];
};