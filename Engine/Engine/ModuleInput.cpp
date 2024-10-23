#include "ModuleInput.h"
#include "App.h"

#include <string>

ModuleInput::ModuleInput(App* app) : Module(app)
{
	for (int i = 0; i < MAX_KEYS; ++i) keyboard[i] = KEY_IDLE;

	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

ModuleInput::~ModuleInput()
{
}

bool ModuleInput::Awake()
{
	LOG(LogType::LOG_INFO, "Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG(LogType::LOG_ERROR, "SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	return ret;
}

bool ModuleInput::PreUpdate(float dt)
{
	bool ret = true;

	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= 1;
	mouse_y /= 1;
	mouse_z = 0;

	for (int i = 0; i < 5; ++i)
	{
		if (buttons & SDL_BUTTON(i))
		{
			if (mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);

		switch (e.type)
		{
		case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

		case SDL_MOUSEMOTION:
			mouse_x = e.motion.x;
			mouse_y = e.motion.y;

			mouse_x_motion = e.motion.xrel;
			mouse_y_motion = e.motion.yrel;
			break;

		case (SDL_DROPFILE):
		{
			std::string droppedFileDir(e.drop.file);

			if (droppedFileDir.substr(droppedFileDir.find(".") + 1) == "fbx"
				|| droppedFileDir.substr(droppedFileDir.find(".") + 1) == "FBX")
			{
				app->renderer3D->meshLoader.ImportFBX(e.drop.file, app->scene->root);
			}
			else if (droppedFileDir.substr(droppedFileDir.find(".") + 1) == "png")
			{
				std::string texturePath;

				if (droppedFileDir.find("Assets/") != std::string::npos)
				{
					texturePath = droppedFileDir.substr(droppedFileDir.find("Assets/"));
				}
				else
				{
					texturePath = droppedFileDir;
				}

				Texture* newTexture = app->renderer3D->LoadTextureImage(texturePath.c_str());
				app->editor->selectedGameObject->material->AddTexture(newTexture);
			}
			else
			{
				LOG(LogType::LOG_WARNING, "File format not supported");
			}

			SDL_free(e.drop.file);
			break;
		}
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				app->renderer3D->OnResize(e.window.data1, e.window.data2);
			break;

		case SDL_QUIT:
			ret = false;
			break;
		}
	}

	return ret;
}

bool ModuleInput::CleanUp()
{
	LOG(LogType::LOG_INFO, "Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}