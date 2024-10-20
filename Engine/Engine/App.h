#ifndef __APP_H__
#define __APP_H__

#pragma once

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"

#include <list>

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleRenderer3D;

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
	
private:
	std::list<Module*> modules;
};

extern App* app;

#endif // !__APP_H__