#pragma once

#include "Module.h"

#include <GL/glew.h>
#include <string>

struct Icons
{
	GLuint folderIcon;
	GLuint openFolderIcon;
	GLuint fileIcon;
	GLuint infoIcon;
	GLuint warningIcon;
	GLuint errorIcon;
};

class ModuleImporter : public Module
{
public:
	ModuleImporter(App* app);
	virtual ~ModuleImporter();

	bool Awake();
	bool CleanUp();

	GLuint LoadTexture(const std::string& filePath);

public:
	Icons icons;
};