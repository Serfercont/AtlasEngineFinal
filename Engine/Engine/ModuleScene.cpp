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

bool ModuleScene::Awake() {
    root = CreateGameObject("Untitled Scene", nullptr);

    sceneLimits = AABB(glm::vec3(-15.0f), glm::vec3(15.0f));
    octreeScene = new Octree(sceneLimits);

    return true;
}

bool ModuleScene::Update(float dt) {
    octreeScene->Clear();

    for (GameObject* gameObject : gameObjects) {
        octreeScene->Insert(gameObject);
        
    }

    if (debugOctree) 
    {
        octreeScene->DrawDebug();
    }
    if (DebugFrust) 
    {
        app->camera->DrawFrustum();
    }
    

    for (auto* gameObject : gameObjects) {
        gameObject->Update();
    }


    return true;
}

bool ModuleScene::CleanUp() {
    delete octreeScene;
    octreeScene = nullptr;

    delete root;
    root = nullptr;

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
