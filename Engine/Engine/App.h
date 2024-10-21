#pragma once

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"

#include "Timer.h"

#include <list>

class App
{
public:
	App(int argc, char* argv[]);
	~App();

	bool Awake();
	bool Start();
	bool Update();
	bool CleanUp();

private:
	void AddModule(Module* module, bool enable = true);

	void PrepareUpdate();
	void FinishUpdate();

public:
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleRenderer3D* renderer3D = nullptr;
	ModuleEditor* editor = nullptr;
	
private:
	Timer	ms_timer;
	float	dt;

	std::list<Module*> modules;
};

extern App* app;