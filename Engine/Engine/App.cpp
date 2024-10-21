#include "App.h"

App::App(int argc, char* argv[])
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);	
	editor = new ModuleEditor(this);

	AddModule(window);
	AddModule(input);
	AddModule(renderer3D);	
	AddModule(editor);
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

	ms_timer.Start();

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
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
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

			ret = module->PreUpdate(dt);
		}
	}

	if (ret)
	{
		for (const auto& module : modules)
		{
			if (!module->active)
				continue;

			ret = module->Update(dt);
		}
	}

	if (ret)
	{
		for (const auto& module : modules)
		{
			if (!module->active)
				continue;

			ret = module->PostUpdate(dt);
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