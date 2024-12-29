#pragma once

#include "Module.h"
#include "Grid.h"
#include "AABB.h"
#include "ComponentCamera.h"
#include "ModuleCamera.h"

#include <SDL2/SDL_video.h>
#include <GL/glew.h>
#include <vector>

#define CHECKERS_WIDTH 128*2
#define CHECKERS_HEIGHT 128*2

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(App* app);
	~ModuleRenderer3D();

	bool Awake();
	bool PreUpdate(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void SetGameMode(bool enabled);

	void SetGameCamera(ComponentCamera* camera);

	void OnResize(int width, int height);
	void CreateFramebuffer();

	bool updateFramebuffer = false;

public:
	GLubyte checkerImage[CHECKERS_WIDTH][CHECKERS_HEIGHT][4];
	unsigned int checkerTextureId;

	Grid grid;

	GLuint fbo;
	GLuint fboTexture;
	GLuint rbo;

private:
	ComponentCamera* gameCamera = nullptr;  // Cámara de juego
	bool isGameMode = false;
};

//#pragma once
//
//#include "Module.h"
//#include "Grid.h"
//#include "AABB.h"
//#include "ModuleTimeManager.h"
//#include "ComponentCamera.h"
//
//#include <SDL2/SDL_video.h>
//#include <GL/glew.h>
//#include <vector>
//
//#define CHECKERS_WIDTH 128*2
//#define CHECKERS_HEIGHT 128*2
//
//class ModuleRenderer3D : public Module
//{
//public:
//	ModuleRenderer3D(App* app);
//	~ModuleRenderer3D();
//
//	bool Awake();
//	bool PreUpdate(float dt);
//	bool PostUpdate(float dt);
//	bool CleanUp();
//
//	//void OnResize(int width, int height);
//
//	bool updateSceneFramebuffer = false;
//	bool updateGameFramebuffer = false;
//
//	// Scene framebuffer
//	uint32_t sceneFBO = 0;
//	uint32_t sceneTextureId = 0;
//	uint32_t sceneRBO = 0;
//
//	// Game framebuffer
//	uint32_t gameFBO = 0;
//	uint32_t gameTextureId = 0;
//	uint32_t gameRBO = 0;
//
//	void CreateSceneFramebuffer();
//	void CreateGameFramebuffer();
//	ComponentCamera* GetActiveCamera() const;
//	void OnSceneResize(float width, float height);
//	void OnGameResize(float width, float height);
//
//private:
//	void RenderToFramebuffer(uint32_t fbo, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
//
//public:
//	GLubyte checkerImage[CHECKERS_WIDTH][CHECKERS_HEIGHT][4];
//	unsigned int checkerTextureId;
//
//	Grid grid;
//
//	GLuint fbo;
//	GLuint fboTexture;
//	GLuint rbo;
//private:
//	//ModuleTimeManager* timeManager;
//	bool IsPlaying = false;
//};