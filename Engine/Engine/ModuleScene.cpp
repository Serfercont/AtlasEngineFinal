#include "ModuleScene.h"
#include "Quadtree.h"
#include "App.h"
#include "GameObject.h"

ModuleScene::ModuleScene(App* app) : Module(app), root(nullptr)
{
    CamScene = new ComponentCamera(nullptr);
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Awake() {
    root = CreateGameObject("Untitled Scene", nullptr);

    GameObject* camera = CreateGameObject("Camera", root);
    MainGameCamera = new ComponentCamera(camera);
	MainGameCamera->isMainCamera = true;
    camera->AddComponent(MainGameCamera);
    camera->transform->position = glm::vec3(0.0f, 6.0f, 8.0f);
    camera->transform->eulerRotation = glm::vec3(-30.0f, 0.0f, 0.0f);
    camera->transform->UpdateTransform();

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
       CamScene->DrawFrustum();
    }
    

	root->Update();


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

ComponentCamera* ModuleScene::GetMainCamera() const {
    for (GameObject* gameObject : gameObjects) {
        if (gameObject && gameObject->camera && gameObject->camera->isMainCamera) {
            return gameObject->camera;
        }
    }
    return nullptr;
}