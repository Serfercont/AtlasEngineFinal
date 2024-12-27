#include "ModuleTimeManager.h"

ModuleTimeManager* ModuleTimeManager::instance = nullptr;

ModuleTimeManager::ModuleTimeManager()
    : gameTime(0.0f)
    , realTimeSinceStartup(0.0f)
    , timeScale(1.0f)
    , isPaused(true)
    , frameCount(0)
    , deltaTime(0.0f)
    , realTimeDeltaTime(0.0f)
    , currentState(GameState::EDITOR_MODE)
    , realTimeStart(std::chrono::steady_clock::now())
    , lastFrameTime(std::chrono::steady_clock::now()) // Añadimos lastFrameTime
{
}

ModuleTimeManager* ModuleTimeManager::GetInstance() {
    if (instance == nullptr) {
        instance = new ModuleTimeManager();
    }
    return instance;
}

void ModuleTimeManager::Initialize() {
    auto now = std::chrono::steady_clock::now();
    realTimeStart = now;
    lastFrameTime = now;
    Reset();
}

void ModuleTimeManager::Update() {
    // Obtener el tiempo actual
    auto currentTime = std::chrono::steady_clock::now();

    // Calcular el delta time real entre frames
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        currentTime - lastFrameTime);
    realTimeDeltaTime = duration.count() / 1000000.0f;

    // Actualizar el tiempo real desde el inicio
    auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(
        currentTime - realTimeStart);
    realTimeSinceStartup = totalDuration.count() / 1000000.0f;

    // Actualizar el tiempo de juego solo si estamos en modo juego y no pausado
    if (currentState == GameState::GAME_MODE && !isPaused) {
        deltaTime = realTimeDeltaTime * timeScale;
        gameTime += deltaTime;
    }
    else {
        deltaTime = 0.0f;
    }

    // Guardar el tiempo actual como último frame
    lastFrameTime = currentTime;
    frameCount++;
}

void ModuleTimeManager::Reset() {
    gameTime = 0.0f;
    auto now = std::chrono::steady_clock::now();
    realTimeStart = now;
    lastFrameTime = now;
    realTimeSinceStartup = 0.0f;
    frameCount = 0;
    deltaTime = 0.0f;
    realTimeDeltaTime = 0.0f;
}

void ModuleTimeManager::Play() {
    isPaused = false;
    currentState = GameState::GAME_MODE;
}

void ModuleTimeManager::Pause() {
    isPaused = true;
}

void ModuleTimeManager::Stop() {
    isPaused = false;
    Reset();
    currentState = GameState::EDITOR_MODE;
}

void ModuleTimeManager::SetTimeScale(float scale) {
    timeScale = scale;
}