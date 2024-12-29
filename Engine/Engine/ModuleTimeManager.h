#pragma once
#include "Module.h"
#include <chrono>


enum class GameState {
    EDITOR_MODE,
    GAME_MODE
};

class ModuleTimeManager : public Module {
public:
    static ModuleTimeManager* GetInstance(App* app = nullptr);


    bool Awake();
    void Initialize();
    void Update();
    void Reset();

    float GetTime() const { return gameTime; }
    float GetTimeScale() const { return timeScale; }
    float GetDeltaTime() const { return deltaTime; }
    float GetRealTimeSinceStartup() const { return realTimeSinceStartup; }
    float GetRealTimeDeltaTime() const { return realTimeDeltaTime; }
    unsigned int GetFrameCount() const { return frameCount; }
    bool IsPlaying() const { return isPlaying; }

    void Play();
    void Pause();
    void Stop();
    void SetTimeScale(float scale);

private:
    explicit ModuleTimeManager(App* app);
    virtual ~ModuleTimeManager();

    static ModuleTimeManager* instance;

    std::chrono::steady_clock::time_point realTimeStart;
public:
    float gameTime;
    float realTimeSinceStartup;
    float timeScale;
    bool isPlaying;
    unsigned int frameCount;
    float deltaTime;
    float realTimeDeltaTime;
protected:
	GameState gameState;
	

};
