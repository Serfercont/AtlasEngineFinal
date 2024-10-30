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
	bool ret = true;

	LookAt(Reference);
	return ret;
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

	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) speed = fastSpeed;

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;
	}

	if (app->input->GetMouseZ() > 0) newPos -= Z * zoomSpeed;
	if (app->input->GetMouseZ() < 0) newPos += Z * zoomSpeed;

	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT &&
		app->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
	{
		int dx = -app->input->GetMouseXMotion();
		int dy = -app->input->GetMouseYMotion();

		float Sensitivity = 0.005f;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotateVector(X, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
			Y = rotateVector(Y, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
			Z = rotateVector(Z, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotateVector(Y, DeltaY, X);
			Z = rotateVector(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = glm::vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = glm::cross(Z, X);
			}
		}
	}

	if ((app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		|| app->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	{
		int dx = -app->input->GetMouseXMotion();
		int dy = -app->input->GetMouseYMotion();

		float Sensitivity = 0.005f;

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotateVector(X, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
			Y = rotateVector(Y, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
			Z = rotateVector(Z, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotateVector(Y, DeltaY, X);
			Z = rotateVector(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = glm::vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = glm::cross(Z, X);
			}
		}

		Position = Reference + Z * length(Position);
		LookAt(Reference);
	}

	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT &&
		app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		int dy = app->input->GetMouseYMotion();

		float Sensitivity = 0.01f;

		if (dy != 0)
		{
			float zoomDelta = (float)dy * Sensitivity;
			glm::vec3 direction = glm::normalize(Position - Reference);
			Position -= direction * zoomDelta;
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		FrameSelected();
	}

	Position += newPos;
	Reference += newPos;

	CalculateViewMatrix();

	return true;
}

void ModuleCamera::FrameSelected()
{
	Position = glm::vec3(0.0f, 5.0f, 5.0f);
	Reference = glm::vec3(0.0f, 0.0f, 0.0f);
	LookAt(Reference);
}

void ModuleCamera::Look(const glm::vec3& Position, const glm::vec3& Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = glm::normalize(Position - Reference);
	X = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Z));
	Y = glm::cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

void ModuleCamera::LookAt(const glm::vec3& Spot)
{
	Reference = Spot;

	Z = glm::normalize(Position - Reference);
	X = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Z));
	Y = glm::cross(Z, X);

	CalculateViewMatrix();
}

void ModuleCamera::Move(const glm::vec3& Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

float* ModuleCamera::GetViewMatrix()
{
	return (float*)&ViewMatrix;
}

void ModuleCamera::CalculateViewMatrix()
{
	ViewMatrix = glm::mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

glm::vec3 ModuleCamera::rotateVector(glm::vec3 const& vector, float angle, glm::vec3 const& axis) {
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);

	glm::vec4 vector4 = glm::vec4(vector, 1.0f);

	glm::vec4 rotatedVector = rotationMatrix * vector4;

	return glm::vec3(rotatedVector);
}