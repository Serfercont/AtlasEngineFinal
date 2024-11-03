#include "ModuleCamera.h"
#include "App.h"

ModuleCamera::ModuleCamera(App* app) : Module(app)
{
	CalculateViewMatrix();

	X = glm::vec3(1.0f, 0.0f, 0.0f);
	Y = glm::vec3(0.0f, 1.0f, 0.0f);
	Z = glm::vec3(0.0f, 0.0f, 1.0f);

	Position = glm::vec3(0.0f, 5.0f, 5.0f);
	Reference = glm::vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera::~ModuleCamera()
{}

bool ModuleCamera::Start()
{
	LOG(LogType::LOG_INFO, "Setting up the camera");

	LookAt(Reference);

	return true;
}

bool ModuleCamera::CleanUp()
{
	LOG(LogType::LOG_INFO, "Cleaning camera");

	return true;
}

bool ModuleCamera::Update(float dt)
{
	glm::vec3 newPos(0, 0, 0);
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

	Position += newPos;
	Reference += newPos;

	CalculateViewMatrix();

	return true;
}

void ModuleCamera::HandleMovement(glm::vec3& newPos, float speed, float fastSpeed)
{
    if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
    {
        if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) speed = fastSpeed;

        if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
        if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;

        if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
        if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;
    }
}

void ModuleCamera::HandleZoom(float zoomSpeed)
{
	int mouseZ = app->input->GetMouseZ();

	if (mouseZ != 0)
		Position -= Z * zoomSpeed * (mouseZ > 0 ? 1.0f : -1.0f);
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

	if ((app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) ||
		app->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	{
		Position -= Reference;
		RotateCamera(dx, dy);
		Position = Reference + Z * glm::length(Position);
		LookAt(Reference);
	}

	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT && app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		float sensitivity = 0.01f;
		float zoomDelta = dy * sensitivity;
		glm::vec3 direction = glm::normalize(Position - Reference);
		Position -= direction * zoomDelta;
	}
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
        Position = glm::vec3(
            app->editor->selectedGameObject->transform->position.x,
            app->editor->selectedGameObject->transform->position.y + 5.0f,
            app->editor->selectedGameObject->transform->position.z + 5.0f
        );
        Reference = app->editor->selectedGameObject->transform->position;
        LookAt(Reference);
    }
	else
	{
		Position = glm::vec3(0.0f, 5.0f, 5.0f);
		Reference = glm::vec3(0.0f, 0.0f, 0.0f);
		LookAt(Reference);
	}
}

void ModuleCamera::LookAt(const glm::vec3& spot)
{
	Reference = spot;

	Z = glm::normalize(Position - Reference);
	X = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Z));
	Y = glm::cross(Z, X);

	CalculateViewMatrix();
}

float* ModuleCamera::GetViewMatrix()
{
	return (float*)&ViewMatrix;
}

void ModuleCamera::CalculateViewMatrix() 
{
	ViewMatrix = glm::mat4(
		X.x, Y.x, Z.x, 0.0f,
		X.y, Y.y, Z.y, 0.0f,
		X.z, Y.z, Z.z, 0.0f,
		-glm::dot(X, Position), -glm::dot(Y, Position), -glm::dot(Z, Position), 1.0f
	);
}

glm::vec3 ModuleCamera::RotateVector(glm::vec3 const& vector, float angle, glm::vec3 const& axis) 
{
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);

	glm::vec4 vector4 = glm::vec4(vector, 1.0f);

	glm::vec4 rotatedVector = rotationMatrix * vector4;

	return glm::vec3(rotatedVector);
}