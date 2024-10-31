#pragma once

#include <string>

#include "imgui.h"

enum class WindowType
{
	HIERARCHY,	
	INSPECTOR,
	CONSOLE,
	PROJECT
};

class EditorWindow 
{
public:
    EditorWindow(const WindowType type, const std::string& name) : type(type), name(name) {}
    virtual ~EditorWindow() {}

    virtual void DrawWindow() = 0;
	WindowType GetType() const { return type; }
    std::string GetName() const { return name; }

protected:
	WindowType type;
    std::string name;
};