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
	if (updateFramebuffer)
	{
		OnResize(app->editor->sceneWindow->windowSize.x, app->editor->sceneWindow->windowSize.y);
		updateFramebuffer = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Usar explícitamente la cámara de juego si estamos en modo juego
	if (isGameMode && gameCamera) {
		glm::mat4 projectionMatrix = gameCamera->GetProjectionMatrix();
		glLoadMatrixf(glm::value_ptr(projectionMatrix));

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glm::mat4 viewMatrix = gameCamera->GetViewMatrix();
		glLoadMatrixf(glm::value_ptr(viewMatrix));
	}
	else {
		// Usar la cámara del editor
		glm::mat4 projectionMatrix = app->camera->GetProjectionMatrix();
		glLoadMatrixf(glm::value_ptr(projectionMatrix));

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glm::mat4 viewMatrix = app->camera->GetViewMatrix();
		glLoadMatrixf(glm::value_ptr(viewMatrix));
	}

	return true;
}

bool ModuleRenderer3D::PostUpdate(float dt)
{
	grid.Render();

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

	// Actualizar dimensiones en la cámara apropiada
	if (isGameMode && gameCamera) {
		// Si estamos en modo juego y hay una cámara de juego activa
		// No necesitamos actualizar screenWidth/Height ya que ComponentCamera
		// usa las dimensiones de la ventana directamente
	}
	else {
		app->camera->screenWidth = width;
		app->camera->screenHeight = height;
	}

	app->window->width = width;
	app->window->height = height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Usar la matriz de proyección apropiada
	glm::mat4 projectionMatrix;
	if (isGameMode && gameCamera) {
		projectionMatrix = gameCamera->GetProjectionMatrix();
	}
	else {
		projectionMatrix = app->camera->GetProjectionMatrix();
	}
	glLoadMatrixf(glm::value_ptr(projectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Recrear el framebuffer
	if (fbo > 0) glDeleteFramebuffers(1, &fbo);
	if (fboTexture > 0) glDeleteTextures(1, &fboTexture);
	if (rbo > 0) glDeleteRenderbuffers(1, &rbo);

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, app->editor->sceneWindow->windowSize.x, app->editor->sceneWindow->windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, app->editor->sceneWindow->windowSize.x, app->editor->sceneWindow->windowSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG(LogType::LOG_ERROR, "Framebuffer is not complete!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//#include "ModuleRenderer3D.h"
//#include "App.h"
//#include "Texture.h"
//#include "AABB.h"
//
//#include <SDL2/SDL_opengl.h>
//#include <gl/GL.h>
//#include <gl/GLU.h>
//#include <IL/il.h>
//#include <IL/ilu.h>
//#include <IL/ilut.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//ModuleRenderer3D::ModuleRenderer3D(App* app) : Module(app), rbo(0), fboTexture(0), fbo(0), checkerTextureId(0)
//{
//	//timeManager = app->timeManager;
//}
//
//ModuleRenderer3D::~ModuleRenderer3D()
//{
//}
//
//bool ModuleRenderer3D::Awake()
//{
//
//	bool ret = true;
//
//	GLenum err = glewInit();
//	if (err != GLEW_OK) {
//		LOG(LogType::LOG_ERROR, "Error in loading Glew: %s\n", glewGetErrorString(err));
//	}
//	else {
//		LOG(LogType::LOG_INFO, "Successfully using Glew %s", glewGetString(GLEW_VERSION));
//	}
//
//	if (ret == true)
//	{
//		glMatrixMode(GL_PROJECTION);
//		glLoadIdentity();
//
//		GLenum error = glGetError();
//		if (error != GL_NO_ERROR)
//		{
//			LOG(LogType::LOG_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
//			ret = false;
//		}
//
//		glMatrixMode(GL_MODELVIEW);
//		glLoadIdentity();
//
//		error = glGetError();
//		if (error != GL_NO_ERROR)
//		{
//			LOG(LogType::LOG_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
//			ret = false;
//		}
//
//		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//		glClearDepth(1.0f);
//
//		glClearColor(0.1f, 0.1f, 0.1f, 1.f);
//
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//		error = glGetError();
//		if (error != GL_NO_ERROR)
//		{
//			LOG(LogType::LOG_ERROR, "Error initializing OpenGL! %s\n", gluErrorString(error));
//			ret = false;
//		}
//
//		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);
//
//		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
//
//		glEnable(GL_DEPTH_TEST);
//		glEnable(GL_CULL_FACE);
//		glEnable(GL_COLOR_MATERIAL);
//		glEnable(GL_TEXTURE_2D);
//
//		ilutRenderer(ILUT_OPENGL);
//	}
//
//	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
//		for (int j = 0; j < CHECKERS_WIDTH; j++) {
//			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
//			checkerImage[i][j][0] = (GLubyte)c;
//			checkerImage[i][j][1] = (GLubyte)c;
//			checkerImage[i][j][2] = (GLubyte)c;
//			checkerImage[i][j][3] = (GLubyte)255;
//		}
//	}
//
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	glGenTextures(1, &checkerTextureId);
//	glBindTexture(GL_TEXTURE_2D, checkerTextureId);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
//
//	app->importer->ImportFile("Assets/Models/BakerHouse.fbx", true);
//	app->editor->selectedGameObject = app->scene->root->children[0];
//
//	CreateSceneFramebuffer();
//	CreateGameFramebuffer();
//	OnSceneResize(SCREEN_WIDTH, SCREEN_HEIGHT);
//	OnGameResize(SCREEN_WIDTH, SCREEN_HEIGHT);
//
//	return ret;
//}
//
//void ModuleRenderer3D::RenderToFramebuffer(uint32_t fbo, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
//{
//	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glLoadMatrixf(glm::value_ptr(projectionMatrix));
//
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	glLoadMatrixf(glm::value_ptr(viewMatrix));
//}
//
//bool ModuleRenderer3D::PreUpdate(float dt)
//{
//	// Actualizar framebuffers si es necesario
//	if (updateSceneFramebuffer && !IsPlaying)
//	{
//		OnSceneResize(app->editor->sceneWindow->windowSize.x, app->editor->sceneWindow->windowSize.y);
//		updateSceneFramebuffer = false;
//	}
//
//	if (updateGameFramebuffer && IsPlaying)
//	{
//		OnGameResize(app->editor->gameWindow->windowSize.x, app->editor->gameWindow->windowSize.y);
//		updateGameFramebuffer = false;
//	}
//
//	// Renderizar la vista de escena
//	RenderToFramebuffer(sceneFBO, app->camera->GetProjectionMatrix(), app->camera->GetViewMatrix());
//
//	// Renderizar la vista de juego si estamos en modo Play
//	if (IsPlaying)
//	{
//		ComponentCamera* gameCamera = GetActiveCamera();
//		if (gameCamera)
//		{
//			RenderToFramebuffer(gameFBO, gameCamera->GetProjectionMatrix(), gameCamera->GetViewMatrix());
//		}
//	}
//
//	return true;
//}
//bool ModuleRenderer3D::PostUpdate(float dt)
//{
//	grid.Render();
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	app->editor->DrawEditor();
//
//	SDL_GL_SwapWindow(app->window->window);
//
//	return true;
//}
//
//bool ModuleRenderer3D::CleanUp()
//{
//	LOG(LogType::LOG_INFO, "Destroying 3D Renderer");
//
//	glDeleteFramebuffers(1, &fbo);
//	glDeleteTextures(1, &fboTexture);
//	glDeleteRenderbuffers(1, &rbo);
//
//	return true;
//}
////
////void ModuleRenderer3D::OnResize(int width, int height)
////{
////	glViewport(0, 0, width, height);
////
////	app->camera->screenWidth = width;
////	app->camera->screenHeight = height;
////
////	app->window->width = width;
////	app->window->height = height;
////
////	glMatrixMode(GL_PROJECTION);
////	glLoadIdentity();
////
////	glm::mat4 projectionMatrix = app->camera->GetProjectionMatrix();
////	glLoadMatrixf(glm::value_ptr(projectionMatrix));
////
////	glMatrixMode(GL_MODELVIEW);
////	glLoadIdentity();
////
////	if (fbo > 0)
////		glDeleteFramebuffers(1, &fbo);
////
////	if (fboTexture > 0)
////		glDeleteTextures(1, &fboTexture);
////
////	if (rbo > 0)
////		glDeleteRenderbuffers(1, &rbo);
////
////	CreateFramebuffer();
////}
//void ModuleRenderer3D::OnSceneResize(float width, float height)
//{
//	if (width > 0 && height > 0) {
//		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
//		glBindTexture(GL_TEXTURE_2D, sceneTextureId);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//
//		glBindRenderbuffer(GL_RENDERBUFFER, sceneRBO);
//		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
//
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	}
//}
//
//void ModuleRenderer3D::OnGameResize(float width, float height)
//{
//	if (width > 0 && height > 0) {
//		glBindFramebuffer(GL_FRAMEBUFFER, gameFBO);
//		glBindTexture(GL_TEXTURE_2D, gameTextureId);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//
//		glBindRenderbuffer(GL_RENDERBUFFER, gameRBO);
//		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
//
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	}
//}
//
//void ModuleRenderer3D::CreateSceneFramebuffer()
//{
//	if (sceneFBO != 0)
//	{
//		glDeleteFramebuffers(1, &sceneFBO);
//		glDeleteTextures(1, &sceneTextureId);
//		glDeleteRenderbuffers(1, &sceneRBO);
//	}
//
//	glGenFramebuffers(1, &sceneFBO);
//	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
//
//	// Generate texture
//	glGenTextures(1, &sceneTextureId);
//	glBindTexture(GL_TEXTURE_2D, sceneTextureId);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTextureId, 0);
//
//	// Generate renderbuffer
//	glGenRenderbuffers(1, &sceneRBO);
//	glBindRenderbuffer(GL_RENDERBUFFER, sceneRBO);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, sceneRBO);
//
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//	{
//		LOG(LogType::LOG_ERROR, "Scene Framebuffer is not complete!");
//	}
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}
//
//void ModuleRenderer3D::CreateGameFramebuffer()
//{
//	if (gameFBO != 0)
//	{
//		glDeleteFramebuffers(1, &gameFBO);
//		glDeleteTextures(1, &gameTextureId);
//		glDeleteRenderbuffers(1, &gameRBO);
//	}
//
//	glGenFramebuffers(1, &gameFBO);
//	glBindFramebuffer(GL_FRAMEBUFFER, gameFBO);
//
//	// Generate texture
//	glGenTextures(1, &gameTextureId);
//	glBindTexture(GL_TEXTURE_2D, gameTextureId);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gameTextureId, 0);
//
//	// Generate renderbuffer
//	glGenRenderbuffers(1, &gameRBO);
//	glBindRenderbuffer(GL_RENDERBUFFER, gameRBO);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gameRBO);
//
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//	{
//		LOG(LogType::LOG_ERROR, "Game Framebuffer is not complete!");
//	}
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}
//
//ComponentCamera* ModuleRenderer3D::GetActiveCamera() const
//{
//	if (!IsPlaying)
//	{
//		for (auto* gameObject : app->scene->GetGameObjects())
//		{
//			if (gameObject->camera && gameObject->camera->isMainCamera)
//			{
//				return gameObject->camera;
//			}
//		}
//	}
//	return nullptr;
//}