#pragma once

#include <string>
#include <vector>

using namespace std;

enum class LogType
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

struct LogInfo
{
	LogType type;
	string message;
};

#define LOG(type, format, ...) Logger::Log(__FILE__, __LINE__, type, format, ## __VA_ARGS__)

class Logger
{
public:
    static void Log(const char file[], int line, LogType type, const char* format, ...);
    void AddLog(LogType type, string message);
    void Clear();

	const vector<LogInfo> GetLogs() const { return logs; }

private:
    vector<LogInfo> logs;
};

extern Logger logger;