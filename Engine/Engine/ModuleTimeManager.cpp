//ModuleTimeManager.cpp
#include "ModuleTimeManager.h"

ModuleTimeManager* ModuleTimeManager::instance = nullptr;

ModuleTimeManager* ModuleTimeManager::GetInstance() {
    if (instance == nullptr) {
        instance = new ModuleTimeManager();
    }
    return instance;
}

ModuleTimeManager::ModuleTimeManager()
    : gameTime(0.0f)
    , realTimeSinceStartup(0.0f)
    , timeScale(1.0f)
    , isPaused(true)
    , frameCount(0)
    , deltaTime(0.0f)
    , realTimeDeltaTime(0.0f) {
}

void ModuleTimeManager::Initialize() {
    realTimeStart = std::chrono::steady_clock::now();
    Reset();
}

void ModuleTimeManager::Update() {

    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(
        currentTime - realTimeStart).count() / 1000000.0f;

    realTimeDeltaTime = elapsedTime - realTimeSinceStartup;
    realTimeSinceStartup = elapsedTime;

    
    if (!isPaused) {
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
    isPaused = false;
}

void ModuleTimeManager::Pause() {
    isPaused = true;
}

void ModuleTimeManager::Stop() {
    isPaused = true;
    Reset();
}

void ModuleTimeManager::SetTimeScale(float scale) {
    timeScale = scale;
}