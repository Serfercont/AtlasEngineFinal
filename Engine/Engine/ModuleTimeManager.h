#pragma once
#include <chrono>

class ModuleTimeManager {
private:
  
    std::chrono::steady_clock::time_point realTimeStart;
    float gameTime;
    float realTimeSinceStartup;

   
    float timeScale;
    bool isPaused;
    unsigned int frameCount;


    float deltaTime;
    float realTimeDeltaTime;

    static ModuleTimeManager* instance;

public:
    static ModuleTimeManager* GetInstance();

    void Initialize();
    void Update();
    void Reset();


    float GetTime() const { return gameTime; }
    float GetTimeScale() const { return timeScale; }
    float GetDeltaTime() const { return deltaTime; }
    float GetRealTimeSinceStartup() const { return realTimeSinceStartup; }
    float GetRealTimeDeltaTime() const { return realTimeDeltaTime; }
    unsigned int GetFrameCount() const { return frameCount; }
    bool IsPaused() const { return isPaused; }

    
    void Play();
    void Pause();
    void Stop();
    void SetTimeScale(float scale);

private:
    ModuleTimeManager();
    ~ModuleTimeManager() = default;
};
