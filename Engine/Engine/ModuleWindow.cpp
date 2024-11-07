#include "ModuleWindow.h"
#include "App.h"

ModuleWindow::ModuleWindow(App* app) : Module(app)
{
	window = NULL;
	screenSurface = NULL;
}

ModuleWindow::~ModuleWindow()
{
}

bool ModuleWindow::Awake()
{
	LOG(LogType::LOG_INFO, "Init SDL window & surface");
	bool ret = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG(LogType::LOG_ERROR, "SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		width = SCREEN_WIDTH;
		height = SCREEN_HEIGHT;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		if (WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if (WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if (WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if (WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow("Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG(LogType::LOG_ERROR, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			screenSurface = SDL_GetWindowSurface(window);
		}

		context = SDL_GL_CreateContext(window);
	}

	return ret;
}

bool ModuleWindow::CleanUp()
{
	LOG(LogType::LOG_INFO, "Destroying SDL window and quitting all SDL systems");

	if (context != NULL)
	{
		SDL_GL_DeleteContext(context);
	}

	if (window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	SDL_Quit();
	return true;
}