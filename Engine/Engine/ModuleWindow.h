#pragma once

#include "Module.h"
#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define WIN_FULLSCREEN 0
#define WIN_FULLSCREEN_DESKTOP 0
#define WIN_BORDERLESS 0
#define WIN_RESIZABLE 1

class ModuleWindow : public Module
{
public:
	ModuleWindow(App* app);
	virtual ~ModuleWindow();

	bool Awake();
	bool CleanUp();

public:
	SDL_Window* window;

	SDL_Surface* screenSurface;
};