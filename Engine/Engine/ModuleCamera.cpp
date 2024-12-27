#include "ModuleCamera.h"
#include "App.h"
#include <algorithm>
#undef max



ImVec2 viewportPos = ImVec2(100, 100); 
ImVec2 viewportSize = ImVec2(800, 600); 
ModuleCamera::ModuleCamera(App* app) : Module(app)
{
	screenWidth = app->window->width;
	screenHeight = app->window->height;

	CalculateViewMatrix();

	X = glm::vec3(1.0f, 0.0f, 0.0f);
	Y = glm::vec3(0.0f, 1.0f, 0.0f);
	Z = glm::vec3(0.0f, 0.0f, 1.0f);

	pos = glm::vec3(0.0f, 5.0f, 5.0f);
	ref = glm::vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera::~ModuleCamera()
{}

bool ModuleCamera::Start()
{
	LOG(LogType::LOG_INFO, "Setting up the camera");

	LookAt(ref);

	return true;
}

bool ModuleCamera::CleanUp()
{
	LOG(LogType::LOG_INFO, "Cleaning camera");

	return true;
}

bool ModuleCamera::Update(float dt)
{
	bool isMouseInside = app->editor->sceneWindow->IsMouseInside();
	if ((!isMouseInside || (!isZooming && !isFreeLook && !isOrbiting && !isDragging)) && !isDefaultCursor)
	{
		SetCursor(CursorType::DEFAULT);
	}

	if (isMouseInside) {
		HandleInput();

		/*if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			SelectGameObject();
		}*/
	}

	CalculateViewMatrix();
	CalculateFrustumPlanes();


	return true;
}

void ModuleCamera::HandleInput()
{
	glm::vec3 newPos(0, 0, 0);

	float dt = app->GetDT();
	float speed = 10.0f * dt;
	float zoomSpeed = 30.0f * dt;
	float fastSpeed = 20.0f * dt;

	HandleMovement(newPos, speed, fastSpeed);

	HandleZoom(zoomSpeed);

	HandleRotation();

	if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		FrameSelected();
	}

	pos += newPos;
	ref += newPos;
}

void ModuleCamera::HandleMovement(glm::vec3& newPos, float speed, float fastSpeed)
{
	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
	{
		if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) speed = fastSpeed;

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

		SetCursor(CursorType::FREELOOK);
	}
	else if (isFreeLook)
		isFreeLook = false;

	if (app->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	{
		int dx = app->input->GetMouseXMotion();
		int dy = app->input->GetMouseYMotion();

		float panSpeed = 0.01f;
		newPos -= X * static_cast<float>(dx) * panSpeed;
		newPos += Y * static_cast<float>(dy) * panSpeed;

		SetCursor(CursorType::DRAG);
	}
	else if (isDragging)
		isDragging = false;
}

void ModuleCamera::HandleZoom(float zoomSpeed)
{
	int mouseZ = app->input->GetMouseZ();

	if (mouseZ != 0)
		pos -= Z * zoomSpeed * (mouseZ > 0 ? 1.0f : -1.0f);
}

void ModuleCamera::HandleRotation()
{
	int dx = -app->input->GetMouseXMotion();
	int dy = -app->input->GetMouseYMotion();

	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT &&
		app->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
	{
		RotateCamera(dx, dy);
	}

	if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		pos -= ref;
		RotateCamera(dx, dy);
		pos = ref + Z * glm::length(pos);
		LookAt(ref);

		SetCursor(CursorType::ORBIT);
	}
	else if (isOrbiting)
		isOrbiting = false;

	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		float sensitivity = 0.01f;
		float zoomDelta = dy * sensitivity;
		glm::vec3 direction = glm::normalize(pos - ref);
		pos += direction * zoomDelta;

		SetCursor(CursorType::ZOOM);
	}
	else if (isZooming)
		isZooming = false;
}

void ModuleCamera::RotateCamera(int dx, int dy)
{
	float sensitivity = 0.005f;

	if (dx != 0)
	{
		float DeltaX = (float)dx * sensitivity;

		X = RotateVector(X, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		Y = RotateVector(Y, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		Z = RotateVector(Z, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * sensitivity;

		Y = RotateVector(Y, DeltaY, X);
		Z = RotateVector(Z, DeltaY, X);

		if (Y.y < 0.0f)
		{
			Z = glm::vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = glm::cross(Z, X);
		}
	}
}

void ModuleCamera::FrameSelected()
{
	if (app->editor->selectedGameObject)
	{
		pos = glm::vec3(
			app->editor->selectedGameObject->transform->position.x,
			app->editor->selectedGameObject->transform->position.y + 5.0f,
			app->editor->selectedGameObject->transform->position.z + 5.0f
		);
		ref = app->editor->selectedGameObject->transform->position;
		LookAt(ref);
	}
	else
	{
		pos = glm::vec3(0.0f, 5.0f, 5.0f);
		ref = glm::vec3(0.0f, 0.0f, 0.0f);
		LookAt(ref);
	}
}

void ModuleCamera::LookAt(const glm::vec3& spot)
{
	ref = spot;

	Z = glm::normalize(pos - ref);
	X = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Z));
	Y = glm::cross(Z, X);

	CalculateViewMatrix();
}

const glm::mat4& ModuleCamera::GetViewMatrix() const {
	return viewMatrix;
}

void ModuleCamera::CalculateViewMatrix() {
	Z = glm::normalize(pos - ref); 
	X = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Z)); 
	Y = glm::cross(Z, X); 

	viewMatrix = glm::mat4(
		X.x, Y.x, Z.x, 0.0f,
		X.y, Y.y, Z.y, 0.0f,
		X.z, Y.z, Z.z, 0.0f,
		-glm::dot(X, pos), -glm::dot(Y, pos), -glm::dot(Z, pos), 1.0f
	);
}

glm::mat4 ModuleCamera::GetProjectionMatrix() const {
	float aspectRatio = app->editor->sceneWindow->windowSize.x / app->editor->sceneWindow->windowSize.y;
	return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

glm::vec3 ModuleCamera::RotateVector(glm::vec3 const& vector, float angle, glm::vec3 const& axis)
{
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);

	glm::vec4 vector4 = glm::vec4(vector, 1.0f);

	glm::vec4 rotatedVector = rotationMatrix * vector4;

	return glm::vec3(rotatedVector);
}

void ModuleCamera::SetCursor(CursorType cursorType)
{
	if (app->input->GetCursor() != cursorType)
	{
		app->input->ChangeCursor(cursorType);

		isDefaultCursor = (cursorType == CursorType::DEFAULT);
		isFreeLook = (cursorType == CursorType::FREELOOK);
		isZooming = (cursorType == CursorType::ZOOM);
		isOrbiting = (cursorType == CursorType::ORBIT);
		isDragging = (cursorType == CursorType::DRAG);
	}
}

void ModuleCamera::CalculateFrustumPlanes() {
	glm::mat4 viewProjectionMatrix = GetProjectionMatrix() * GetViewMatrix();

	const int axisIndices[6] = { 0, 0, 1, 1, 2, 2 };
	const float planeSigns[6] = { 1, -1, 1, -1, 1, -1 };

	for (int i = 0; i < 6; ++i) {
		int axis = axisIndices[i];
		float sign = planeSigns[i];

		frustumPlanes[i].normal.x = viewProjectionMatrix[0][3] + sign * viewProjectionMatrix[0][axis];
		frustumPlanes[i].normal.y = viewProjectionMatrix[1][3] + sign * viewProjectionMatrix[1][axis];
		frustumPlanes[i].normal.z = viewProjectionMatrix[2][3] + sign * viewProjectionMatrix[2][axis];
		frustumPlanes[i].distance = viewProjectionMatrix[3][3] + sign * viewProjectionMatrix[3][axis];

		float length = glm::length(frustumPlanes[i].normal);
		frustumPlanes[i].normal /= length;
		frustumPlanes[i].distance /= length;
	}
}


bool ModuleCamera::IsBoxInsideFrustum(const AABB& box) const
{
	for (int i = 0; i < 6; ++i)
	{
		const Plane& currentPlane = frustumPlanes[i];
		bool isAnyCornerInside = false;

		glm::vec3 corners[8] = {
			{ box.minPoint.x, box.minPoint.y, box.minPoint.z },
			{ box.maxPoint.x, box.minPoint.y, box.minPoint.z },
			{ box.minPoint.x, box.maxPoint.y, box.minPoint.z },
			{ box.maxPoint.x, box.maxPoint.y, box.minPoint.z },
			{ box.minPoint.x, box.minPoint.y, box.maxPoint.z },
			{ box.maxPoint.x, box.minPoint.y, box.maxPoint.z },
			{ box.minPoint.x, box.maxPoint.y, box.maxPoint.z },
			{ box.maxPoint.x, box.maxPoint.y, box.maxPoint.z }
		};

		for (int cornerIndex = 0; cornerIndex < 8; ++cornerIndex)
		{
			float distanceToPlane = glm::dot(currentPlane.normal, corners[cornerIndex]) + currentPlane.distance;
			if (distanceToPlane >= 0)
			{
				isAnyCornerInside = true;
				break;
			}
		}

		if (!isAnyCornerInside) 
		{
			return false;
		}
	}

	return true;
}

void ModuleCamera::DrawFrustum() {
	glm::mat4 vpMatrix = GetProjectionMatrix() * GetViewMatrix();
	glm::mat4 invVPMatrix = glm::inverse(vpMatrix);

	glm::vec4 corners[8] = {
		{-1, -1, -1, 1}, {1, -1, -1, 1},
		{1,  1, -1, 1}, {-1,  1, -1, 1},
		{-1, -1,  1, 1}, {1, -1,  1, 1},
		{1,  1,  1, 1}, {-1,  1,  1, 1}
	};

	for (auto& corner : corners) {
		corner = invVPMatrix * corner;
		corner /= corner.w;
	}

	glPushAttrib(GL_ENABLE_BIT); 
	glDisable(GL_LIGHTING);      
	glLineWidth(2.0f);           

	glBegin(GL_LINES);
	for (int i = 0; i < 4; ++i) {
		glVertex3fv(&corners[i].x); glVertex3fv(&corners[(i + 1) % 4].x);      
		glVertex3fv(&corners[i + 4].x); glVertex3fv(&corners[((i + 1) % 4) + 4].x); 
		glVertex3fv(&corners[i].x); glVertex3fv(&corners[i + 4].x);          
	}
	glEnd();

	glPopAttrib(); 
}


void ModuleCamera::OnWindowResized(int newWidth, int newHeight) {
    screenWidth = newWidth;
    screenHeight = newHeight;
}
//Ray ModuleCamera::CreateRayFromMouse()
//{
//	// Obtén la posición normalizada del ratón
//	glm::vec2 normalizedMousePos = app->input->GetNormalizedMousePosition(viewportPos, viewportSize);
//
//	// Calcula la matriz de vista-proyección inversa
//	glm::mat4 viewMatrix = GetViewMatrix();
//	glm::mat4 projectionMatrix = GetProjectionMatrix();
//	glm::mat4 vpMatrix = projectionMatrix * viewMatrix;
//	glm::mat4 invVP = glm::inverse(vpMatrix);
//
//	// Calcula los puntos cercanos y lejanos en espacio de mundo
//	glm::vec4 nearPoint = invVP * glm::vec4(normalizedMousePos.x, normalizedMousePos.y, -1.0f, 1.0f);
//	glm::vec4 farPoint = invVP * glm::vec4(normalizedMousePos.x, normalizedMousePos.y, 1.0f, 1.0f);
//
//	// Normaliza las coordenadas
//	nearPoint /= nearPoint.w;
//	farPoint /= farPoint.w;
//
//	// Crea el rayo
//	Ray ray;
//	ray.origin = glm::vec3(nearPoint);
//	ray.direction = glm::normalize(glm::vec3(farPoint - nearPoint));
//	
//
//	return ray;
//}
//
//void ModuleCamera::SelectGameObject()
//{
//	Ray ray = CreateRayFromMouse();
//	GameObject* closestObject = nullptr;
//	float closestDistance = FLT_MAX;
//
//	for (GameObject* obj : app->scene->GetGameObjects())
//	{
//		if (AABB::RayIntersectsAABB(ray, obj->GetAABB()))
//		{
//			float distance = glm::length(ray.origin - obj->transform->position);
//			if (distance < closestDistance)
//			{
//				closestDistance = distance;
//				closestObject = obj;
//			}
//		}
//	}
//
//	// Actualiza el objeto seleccionado en el editor
//	if (closestObject)
//	{
//		app->editor->selectedGameObject = closestObject;
//		printf("Selected object: %s\n", closestObject->name.c_str());
//	}
//}
//
//glm::vec3 ModuleCamera::ScreenToWorldRay(float x, float y)
//{
//	// Asumiendo que tienes una matriz de proyección y vista
//	glm::mat4 projectionMatrix = app->camera->GetProjectionMatrix();
//	glm::mat4 viewMatrix = app->camera->GetViewMatrix();
//
//	// Calcula las coordenadas normalizadas de dispositivo (NDC) en el rango [-1, 1]
//	float normalizedX = (2.0f * x) / app->window->width - 1.0f;
//	float normalizedY = 1.0f - (2.0f * y) / app->window->height;
//	glm::vec4 rayClip(normalizedX, normalizedY, -1.0f, 1.0f);
//
//	// Convierte de espacio de clip a espacio de cámara
//	glm::mat4 invProjectionView = glm::inverse(projectionMatrix * viewMatrix);
//	glm::vec4 rayEye = invProjectionView * rayClip;
//	rayEye.z = -1.0f;
//	rayEye.w = 0.0f;
//
//	// Convierte de espacio de cámara a espacio mundial
//	glm::vec3 rayWorld = glm::vec3(invProjectionView * rayEye);
//	return glm::normalize(rayWorld); // Normaliza para obtener la dirección del rayo
//}
//
//
