#include "ModuleRenderer3D.h"
#include "App.h"
#include "Texture.h"

#include <SDL2/SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


ModuleRenderer3D::ModuleRenderer3D(App* app) : Module(app)
{
}


ModuleRenderer3D::~ModuleRenderer3D()
{
}


bool ModuleRenderer3D::Awake()
{
	LOG(LogType::LOG_INFO, "Creating 3D Renderer context");
	bool ret = true;

	meshLoader.EnableDebugger();

	context = SDL_GL_CreateContext(app->window->window);
	if (context == NULL)
	{
		LOG(LogType::LOG_ERROR, "OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

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

		ilInit();
		iluInit();
		ilutInit();
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

	meshLoader.ImportFBX("Assets/Models/BakerHouse.fbx", app->scene->root);
	app->editor->selectedGameObject = app->scene->root->children[0];

	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return ret;
}

bool ModuleRenderer3D::PreUpdate(float dt)
{
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

	app->editor->DrawEditor();

	SDL_GL_SwapWindow(app->window->window);

	return true;
}

bool ModuleRenderer3D::CleanUp()
{
	LOG(LogType::LOG_INFO, "Destroying 3D Renderer");

	meshLoader.DisableDebugger();

	for (unsigned int i = 0; i < mesh.size(); i++)
	{
		mesh[i]->CleanUpMesh();
	}

	SDL_GL_DeleteContext(context);

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
}

Texture* ModuleRenderer3D::LoadTextureImage(const char* file)
{
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);

	if (!ilLoadImage(file))
	{
		LOG(LogType::LOG_WARNING, "Image not loaded");
		return nullptr;
	}

	Texture* newTexture = new Texture(ilutGLBindTexImage(), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), file);

	ilDeleteImages(1, &image);

	return newTexture;
}