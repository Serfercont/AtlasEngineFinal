#include "ModuleRenderer3D.h"
#include "App.h"
#include "Texture.h"
#include "AABB.h"

#include <SDL2/SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

ModuleRenderer3D::ModuleRenderer3D(App* app) : Module(app), rbo(0), fboTexture(0), fbo(0), checkerTextureId(0)
{
}

ModuleRenderer3D::~ModuleRenderer3D()
{
}

bool ModuleRenderer3D::Awake()
{
	bool ret = true;

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		LOG(LogType::LOG_ERROR, "Error in loading Glew: %s\n", glewGetErrorString(err));
	}
	else {
		LOG(LogType::LOG_INFO, "Successfully using Glew %s", glewGetString(GLEW_VERSION));
	}

	if (ret == true)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG(LogType::LOG_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG(LogType::LOG_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		glClearColor(0.1f, 0.1f, 0.1f, 1.f);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG(LogType::LOG_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

		ilutRenderer(ILUT_OPENGL);
	}

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkerTextureId);
	glBindTexture(GL_TEXTURE_2D, checkerTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	app->importer->ImportFile("Assets/Models/BakerHouse.fbx", true);
	app->editor->selectedGameObject = app->scene->root->children[0];

	CreateFramebuffer();

	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return ret;
}

bool ModuleRenderer3D::PreUpdate(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glm::mat4 projectionMatrix = app->camera->GetProjectionMatrix();
	glLoadMatrixf(glm::value_ptr(projectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glm::mat4 viewMatrix = app->camera->GetViewMatrix();
	glLoadMatrixf(glm::value_ptr(viewMatrix));

	return true;
}

bool ModuleRenderer3D::PostUpdate(float dt)
{
	grid.Render();

	std::vector<GameObject*> gameObjects = app->scene->GetGameObjects();
	//LOG(LogType::LOG_INFO, "Number of GameObjects: %d", gameObjects.size());

	for (GameObject* go : gameObjects) {
		ComponentMesh* mesh = static_cast<ComponentMesh*>(go->GetComponent(ComponentType::MESH));
		if (mesh) {
			LOG(LogType::LOG_INFO, "Found a GameObject with a mesh component.");
			RenderAABB(mesh->localAABB, go->transform->globalTransform);
		}
		else {
			LOG(LogType::LOG_WARNING, "GameObject without mesh component.");
		}

	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	app->editor->DrawEditor();

	SDL_GL_SwapWindow(app->window->window);

	return true;
}

bool ModuleRenderer3D::CleanUp()
{
	LOG(LogType::LOG_INFO, "Destroying 3D Renderer");

	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &fboTexture);
	glDeleteRenderbuffers(1, &rbo);

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	app->camera->screenWidth = width;
	app->camera->screenHeight = height;

	app->window->width = width;
	app->window->height = height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glm::mat4 projectionMatrix = app->camera->GetProjectionMatrix();
	glLoadMatrixf(glm::value_ptr(projectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (fbo > 0)
		glDeleteFramebuffers(1, &fbo);

	if (fboTexture > 0)
		glDeleteTextures(1, &fboTexture);

	if (rbo > 0)
		glDeleteRenderbuffers(1, &rbo);

	CreateFramebuffer();
}

void ModuleRenderer3D::CreateFramebuffer()
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &fboTexture);
	glBindTexture(GL_TEXTURE_2D, fboTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, app->window->width, app->window->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, app->window->width, app->window->height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG(LogType::LOG_ERROR, "Framebuffer is not complete!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ModuleRenderer3D::RenderAABB(const AABB& aabb, const glm::mat4& transform) {
	LOG(LogType::LOG_INFO, "RenderAABB called.");

	// Calcular las esquinas transformadas del AABB
	glm::vec3 corners[8] = {
		glm::vec3(transform * glm::vec4(aabb.minPoint.x, aabb.minPoint.y, aabb.minPoint.z, 1.0f)),
		glm::vec3(transform * glm::vec4(aabb.maxPoint.x, aabb.minPoint.y, aabb.minPoint.z, 1.0f)),
		glm::vec3(transform * glm::vec4(aabb.minPoint.x, aabb.maxPoint.y, aabb.minPoint.z, 1.0f)),
		glm::vec3(transform * glm::vec4(aabb.maxPoint.x, aabb.maxPoint.y, aabb.minPoint.z, 1.0f)),
		glm::vec3(transform * glm::vec4(aabb.minPoint.x, aabb.minPoint.y, aabb.maxPoint.z, 1.0f)),
		glm::vec3(transform * glm::vec4(aabb.maxPoint.x, aabb.minPoint.y, aabb.maxPoint.z, 1.0f)),
		glm::vec3(transform * glm::vec4(aabb.minPoint.x, aabb.maxPoint.y, aabb.maxPoint.z, 1.0f)),
		glm::vec3(transform * glm::vec4(aabb.maxPoint.x, aabb.maxPoint.y, aabb.maxPoint.z, 1.0f))
	};

	for (int i = 0; i < 8; ++i) {
		LOG(LogType::LOG_INFO, "Corner %d: (%.2f, %.2f, %.2f)", i, corners[i].x, corners[i].y, corners[i].z);
	}

	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); // Rojo para debug

	// Dibujar l�neas entre esquinas
	int indices[] = { 0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7, 7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7 };
	for (int i = 0; i < 24; i += 2) {
		glVertex3f(corners[indices[i]].x, corners[indices[i]].y, corners[indices[i]].z);
		glVertex3f(corners[indices[i + 1]].x, corners[indices[i + 1]].y, corners[indices[i + 1]].z);

		LOG(LogType::LOG_INFO, "Line from (%.2f, %.2f, %.2f) to (%.2f, %.2f, %.2f)",
			corners[indices[i]].x, corners[indices[i]].y, corners[indices[i]].z,
			corners[indices[i + 1]].x, corners[indices[i + 1]].y, corners[indices[i + 1]].z);
	}

	glEnd();

	// Verificar errores de OpenGL
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		LOG(LogType::LOG_ERROR, "OpenGL error in RenderAABB: %s", gluErrorString(error));
	}
}
