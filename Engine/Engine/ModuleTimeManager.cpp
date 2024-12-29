#include "ModuleTimeManager.h"
#include "App.h"

// Inicialización de la instancia singleton
ModuleTimeManager* ModuleTimeManager::instance = nullptr;

ModuleTimeManager::ModuleTimeManager(App* app)
    : Module(app),
    gameTime(0.0f),
    realTimeSinceStartup(0.0f),
    timeScale(1.0f),
    isPlaying(false),
    frameCount(0),
    deltaTime(0.0f),
    realTimeDeltaTime(0.0f),
    realTimeStart(std::chrono::steady_clock::now()) {
}

ModuleTimeManager::~ModuleTimeManager() {
    delete instance;
}

ModuleTimeManager* ModuleTimeManager::GetInstance(App* app) {
    if (instance == nullptr) {
        // Asegurarse de que app no sea nulo al inicializar por primera vez
        if (app == nullptr) {
            throw std::runtime_error("App* cannot be null when initializing ModuleTimeManager");
        }
        instance = new ModuleTimeManager(app);
    }
    return instance;
}

bool ModuleTimeManager::Awake() {
    Initialize();
    return true;
}

void ModuleTimeManager::Initialize() {
    realTimeStart = std::chrono::steady_clock::now();
    Reset();
}

void ModuleTimeManager::Update() {
    auto currentTime = std::chrono::steady_clock::now();
    float elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(
        currentTime - realTimeStart).count() / 1000000.0f;

    realTimeDeltaTime = elapsedTime - realTimeSinceStartup;
    realTimeSinceStartup = elapsedTime;

    if (isPlaying) {
        deltaTime = realTimeDeltaTime * timeScale;
        gameTime += deltaTime;
    }
    else {
        deltaTime = 0.0f;
    }

    frameCount++;
}

void ModuleTimeManager::Reset() {
    gameTime = 0.0f;
    realTimeStart = std::chrono::steady_clock::now();
    realTimeSinceStartup = 0.0f;
    frameCount = 0;
    deltaTime = 0.0f;
    realTimeDeltaTime = 0.0f;
}

void ModuleTimeManager::Play() {
    isPlaying = true;
}

void ModuleTimeManager::Pause() {
    isPlaying = false;
}

void ModuleTimeManager::Stop() {
    isPlaying = false;
    Reset();
}

void ModuleTimeManager::SetTimeScale(float scale) {
    timeScale = scale;
}
