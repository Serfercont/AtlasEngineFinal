#include "App.h"

App::App(int argc, char* argv[])
{
	window = new ModuleWindow();
	input = new ModuleInput();
	renderer3D = new ModuleRenderer3D();

	AddModule(window);
	AddModule(input);
	AddModule(renderer3D);
}

App::~App()
{
	modules.clear();
}

bool App::Awake()
{
	bool ret = true;

	for (const auto& module : modules)
	{
		if (!module->active)
			continue;

		ret = module->Awake();
	}

	return ret;
}

bool App::Start()
{
	bool ret = true;

	for (const auto& module : modules)
	{
		if (!module->active)
			continue;

		ret = module->Start();
	}

	return ret;
}

void App::PrepareUpdate()
{

}

bool App::Update()
{
	bool ret = true;

	PrepareUpdate();

	if (ret)
	{
		for (const auto& module : modules)
		{
			if (!module->active)
				continue;

			ret = module->PreUpdate();
		}
	}

	if (ret)
	{
		for (const auto& module : modules)
		{
			if (!module->active)
				continue;

			ret = module->Update(0.0f);
		}
	}

	if (ret)
	{
		for (const auto& module : modules)
		{
			if (!module->active)
				continue;

			ret = module->PostUpdate();
		}
	}

	FinishUpdate();

	return ret;
}

void App::FinishUpdate()
{

}

bool App::CleanUp()
{
	bool ret = true;

	for (auto it = modules.rbegin(); it != modules.rend(); ++it)
	{
		ret = (*it)->CleanUp();
	}

	return ret;
}

void App::AddModule(Module* module, bool activate)
{
	if (activate)
		module->Init();

	modules.push_back(module);
}