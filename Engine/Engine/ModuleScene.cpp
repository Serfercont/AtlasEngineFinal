#include "ModuleScene.h"
#include "Quadtree.h"
#include "App.h"
#include "GameObject.h"

ModuleScene::ModuleScene(App* app) : Module(app), root(nullptr)
{

}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Awake()
{
	root = CreateGameObject("Untitled Scene", nullptr);

	sceneLimits=AABB(glm::vec3(-15.0f), glm::vec3(15.0f));
	quadtreeScene = new Quadtree(sceneLimits);

	return true;
}

bool ModuleScene::Update(float dt)
{
	for (GameObject* gameObject : gameObjects)
    {
        gameObject->Update();
		quadtreeScene->Insert(gameObject);
    }

	if (debugQuadtree) {
		quadtreeScene->DrawDebug();
	}

	for (auto* gameObject : gameObjects) {
		gameObject->Update();
	}

	return true;
}

bool ModuleScene::CleanUp()
{
	LOG(LogType::LOG_INFO, "Cleaning ModuleScene");

	delete quadtreeScene;
	quadtreeScene = nullptr;

	for (auto* gameObject : gameObjects) {
		delete gameObject;
	}
	gameObjects.clear();

	return true;
}


GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* gameObject = new GameObject(name, parent);

	if (parent != nullptr) parent->children.push_back(gameObject);

	gameObjects.push_back(gameObject);

	return gameObject;
}

std::vector<GameObject*>& ModuleScene::GetGameObjects()
{
	return gameObjects;
}
