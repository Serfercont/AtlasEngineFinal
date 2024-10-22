#include "ModuleCamera.h"
#include "App.h"

ModuleCamera::ModuleCamera(App* app) : Module(app)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 5.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
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
	vec3 newPos(0, 0, 0);
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

			X = rotateVector(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotateVector(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotateVector(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotateVector(Y, DeltaY, X);
			Z = rotateVector(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
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

			X = rotateVector(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotateVector(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotateVector(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotateVector(Y, DeltaY, X);
			Z = rotateVector(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
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
			vec3 direction = normalize(Position - Reference);
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
	Position = vec3(0.0f, 5.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);
	LookAt(Reference);
}

void ModuleCamera::Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

void ModuleCamera::LookAt(const vec3& Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}

void ModuleCamera::Move(const vec3& Movement)
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
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

vec3 ModuleCamera::rotateVector(vec3 const& vector, float angle, vec3 const& axis) {
	mat4 rotationMatrix = rotate(mat4(1.0f), angle, axis);

	vec4 vector4 = vec4(vector, 1.0f);

	vec4 rotatedVector = rotationMatrix * vector4;

	return vec3(rotatedVector);
}