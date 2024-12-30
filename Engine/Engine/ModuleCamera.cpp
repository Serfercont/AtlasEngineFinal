#include "ModuleCamera.h"
#include "App.h"
#include "glm/gtc/matrix_transform.hpp"
#include <algorithm>
#undef max


ModuleCamera::ModuleCamera(App* app) : Module(app)
{
	
}

ModuleCamera::~ModuleCamera()
{}

bool ModuleCamera::Start()
{
	LOG(LogType::LOG_INFO, "Setting up the camera");

	LookAt(app->scene->CamScene->reference);

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

	}

	app->scene->CamScene->Update();

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

	app->scene->CamScene->position += newPos;
	app->scene->CamScene->reference += newPos;

	if (glm::length(newPos) > 0.0f)
		app->scene->CamScene->CalculateFrust=true;

}

void ModuleCamera::HandleMovement(glm::vec3& newPos, float speed, float fastSpeed)
{
	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
	{
		if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) speed = fastSpeed;

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= app->scene->CamScene->Z * speed;
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += app->scene->CamScene->Z * speed;

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= app->scene->CamScene->X * speed;
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += app->scene->CamScene->X * speed;

		SetCursor(CursorType::FREELOOK);
	}
	else if (isFreeLook)
		isFreeLook = false;

	if (app->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	{
		int dx = app->input->GetMouseXMotion();
		int dy = app->input->GetMouseYMotion();

		float panSpeed = 0.01f;
		newPos -= app->scene->CamScene->X * static_cast<float>(dx) * panSpeed;
		newPos += app->scene->CamScene->Y * static_cast<float>(dy) * panSpeed;

		SetCursor(CursorType::DRAG);
		app->scene->CamScene->CalculateFrust=true;
	}
	else if (isDragging)
		isDragging = false;
}

void ModuleCamera::HandleZoom(float zoomSpeed)
{
	int mouseZ = app->input->GetMouseZ();

	if (mouseZ != 0)
		app->scene->CamScene->position -= app->scene->CamScene->Z * zoomSpeed * (mouseZ > 0 ? 1.0f : -1.0f);

		app->scene->CamScene->CalculateFrust = true;
}

void ModuleCamera::HandleRotation()
{
	int dx = -app->input->GetMouseXMotion();
	int dy = -app->input->GetMouseYMotion();

	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT &&
		app->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
	{
		RotateCamera(dx, dy);
		app->scene->CamScene->CalculateFrust = true;
	}

	if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		app->scene->CamScene->position -= app->scene->CamScene->reference;
		RotateCamera(dx, dy);
		app->scene->CamScene->position = app->scene->CamScene->reference + app->scene->CamScene->Z * glm::length(app->scene->CamScene->position);

		LookAt(app->scene->CamScene->reference);

		SetCursor(CursorType::ORBIT);
		app->scene->CamScene->CalculateFrust = true;
	}
	else if (isOrbiting)
		isOrbiting = false;

	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		float sensitivity = 0.01f;
		float zoomDelta = dy * sensitivity;
		glm::vec3 direction = glm::normalize(app->scene->CamScene->position - app->scene->CamScene->reference);
		app->scene->CamScene->position += direction * zoomDelta;

		SetCursor(CursorType::ZOOM);
		app->scene->CamScene->CalculateFrust = true;
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

		app->scene->CamScene->X = RotateVector(app->scene->CamScene->X, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		app->scene->CamScene->Y = RotateVector(app->scene->CamScene->Y, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		app->scene->CamScene->Z = RotateVector(app->scene->CamScene->Z, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * sensitivity;

		app->scene->CamScene->Y = RotateVector(app->scene->CamScene->Y, DeltaY, app->scene->CamScene->X);
		app->scene->CamScene->Z = RotateVector(app->scene->CamScene->Z, DeltaY, app->scene->CamScene->X);

		if (Y.y < 0.0f)
		{
			app->scene->CamScene->Z = glm::vec3(0.0f, app->scene->CamScene->Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			app->scene->CamScene->Y = glm::cross(app->scene->CamScene->Z, app->scene->CamScene->X);
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
	app->scene->CamScene->reference = spot;

	app->scene->CamScene->Z = glm::normalize(app->scene->CamScene->position - app->scene->CamScene->reference);
	app->scene->CamScene->X = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), app->scene->CamScene->Z));
	app->scene->CamScene->Y = glm::cross(app->scene->CamScene->Z, app->scene->CamScene->X);

	app->scene->CamScene->CalculateViewMatrix();
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



void ModuleCamera::OnWindowResized(int newWidth, int newHeight) {
    screenWidth = newWidth;
    screenHeight = newHeight;
}
