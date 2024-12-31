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

ModuleRenderer3D::ModuleRenderer3D(App* app) : Module(app), sceneRBO(0), sceneTextureId(0), sceneFBO(0), checkerTextureId(0)
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
	if (updateFramebuffer)
	{
		OnResize(app->editor->sceneWindow->windowSize.x, app->editor->sceneWindow->windowSize.y);
		updateFramebuffer = false;
	}
	return true;
}

bool ModuleRenderer3D::PostUpdate(float dt)
{
	// Render first camera
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glm::mat4 projectionMatrix = app->scene->CamScene->GetProjectionMatrix();
	glLoadMatrixf(glm::value_ptr(projectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glm::mat4 viewMatrix = app->scene->CamScene->GetViewMatrix();
	glLoadMatrixf(glm::value_ptr(viewMatrix));

	grid.Render();

	DrawQueuedMeshes(app->scene->CamScene);

	if (app->scene->debugOctree)
	{
		app->scene->octreeScene->DrawDebug();
	}
	if (app->scene->DebugFrust)
	{
		app->scene->CamScene->DrawFrustum();
	}

	if (app->scene->MainGameCamera)
	{
		// Render second camera
		glBindFramebuffer(GL_FRAMEBUFFER, gameFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		projectionMatrix = app->scene->MainGameCamera->GetProjectionMatrix();
		glLoadMatrixf(glm::value_ptr(projectionMatrix));

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		viewMatrix = app->scene->MainGameCamera->GetViewMatrix();
		glLoadMatrixf(glm::value_ptr(viewMatrix));

		DrawQueuedMeshes(app->scene->MainGameCamera);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	app->editor->DrawEditor();

	SDL_GL_SwapWindow(app->window->window);

	queuedMeshes.clear();

	return true;
}
void ModuleRenderer3D::DrawQueuedMeshes(ComponentCamera* camera)
{
	for (ComponentMesh* mesh : queuedMeshes)
	{
		if (!mesh || !mesh->mesh || !mesh->gameObject || !mesh->gameObject->transform){}
		mesh->DrawMeshes(camera);
	}
}
bool ModuleRenderer3D::CleanUp()
{
	LOG(LogType::LOG_INFO, "Destroying 3D Renderer");

	glDeleteFramebuffers(1, &sceneFBO);
	glDeleteTextures(1, &sceneTextureId);
	glDeleteRenderbuffers(1, &sceneRBO);

	glDeleteFramebuffers(1, &gameFBO);
	glDeleteTextures(1, &gameTextureId);
	glDeleteRenderbuffers(1, &gameRBO);

	return true;
}

void ModuleRenderer3D::SetGameMode(bool enabled) {
	isGameMode = enabled;
	if (enabled && gameCamera == nullptr) {
		// Intentar obtener la cámara principal
		gameCamera = app->scene->GetMainCamera();
	}
}

void ModuleRenderer3D::SetGameCamera(ComponentCamera* camera) {
	gameCamera = camera;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	app->scene->CamScene->screenWidth = width;
	app->scene->CamScene->screenHeight = height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glm::mat4 projectionMatrix = app->scene->CamScene->GetProjectionMatrix();
	glLoadMatrixf(glm::value_ptr(projectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (sceneFBO > 0)
		glDeleteFramebuffers(1, &sceneFBO);

	if (sceneTextureId > 0)
		glDeleteTextures(1, &sceneTextureId);

	if (sceneRBO > 0)
		glDeleteRenderbuffers(1, &sceneRBO);

	if (gameFBO > 0)
		glDeleteFramebuffers(1, &gameFBO);

	if (gameTextureId > 0)
		glDeleteTextures(1, &gameTextureId);

	if (gameRBO > 0)
		glDeleteRenderbuffers(1, &gameRBO);

	CreateFramebuffer();
}

void ModuleRenderer3D::CreateFramebuffer()
{
	glGenFramebuffers(1, &sceneFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

	glGenTextures(1, &sceneTextureId);
	glBindTexture(GL_TEXTURE_2D, sceneTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, app->editor->sceneWindow->windowSize.x, app->editor->sceneWindow->windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTextureId, 0);

	glGenRenderbuffers(1, &sceneRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, sceneRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, app->editor->sceneWindow->windowSize.x, app->editor->sceneWindow->windowSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneRBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG(LogType::LOG_ERROR, "Scene Framebuffer is not complete!");
	}

	glGenFramebuffers(1, &gameFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gameFBO);

	glGenTextures(1, &gameTextureId);
	glBindTexture(GL_TEXTURE_2D, gameTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, app->editor->sceneWindow->windowSize.x, app->editor->sceneWindow->windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gameTextureId, 0);

	glGenRenderbuffers(1, &gameRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, gameRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, app->editor->sceneWindow->windowSize.x, app->editor->sceneWindow->windowSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gameRBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG(LogType::LOG_ERROR, "Game Framebuffer is not complete!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
