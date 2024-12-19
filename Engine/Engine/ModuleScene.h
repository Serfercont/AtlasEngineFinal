#pragma once

#include "Module.h"
#include "GameObject.h"
#include "Quadtree.h"

class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene(App* app);
	virtual ~ModuleScene();

	bool Awake();
	bool Update(float dt);
	bool CleanUp();

	GameObject* CreateGameObject(const char* name, GameObject* parent);

	std::vector<GameObject*>& GetGameObjects();

public:
	GameObject* root = nullptr;
	std::vector<GameObject*> gameObjects;
	Quadtree* quadtreeScene = nullptr; 
	AABB sceneLimits;
	bool debugQuadtree = true;   

};