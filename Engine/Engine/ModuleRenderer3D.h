#pragma once

#include "Module.h"
#include "Grid.h"
#include "AABB.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
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
	void DrawQueuedMeshes(ComponentCamera* camera);
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

	GLuint sceneFBO;
	GLuint sceneTextureId;
	GLuint sceneRBO;
	
	// Game framebuffer
	GLuint gameFBO;
	GLuint gameTextureId ;
	GLuint gameRBO;

	std::vector<ComponentMesh*> queuedMeshes;

private:
	ComponentCamera* gameCamera = nullptr;  // Cámara de juego
	bool isGameMode = false;
};
