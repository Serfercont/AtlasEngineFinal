#pragma once

#include "Module.h"

#include <GL/glew.h>
#include <string>

struct Icons
{
	GLuint folderIcon = 0;
	GLuint openFolderIcon = 0;
	GLuint fileIcon = 0;
	GLuint pngFileIcon = 0;
	GLuint ddsFileIcon = 0;
	GLuint fbxFileIcon = 0;
	GLuint dotsIcon = 0;
	GLuint infoIcon = 0;
	GLuint warningIcon = 0;
	GLuint errorIcon = 0;
};

class ModuleImporter : public Module
{
public:
	ModuleImporter(App* app);
	virtual ~ModuleImporter();

	bool Awake();
	bool CleanUp();

	GLuint LoadTexture(const std::string& filePath);

	std::string OpenFileDialog(const char* filter);

	void TryImportFile();

	void ImportFile(const std::string& fileDir, bool addToScene = false);

	void SetDraggedFile(const std::string& filePath);

public:
	Icons icons;
	bool isDraggingFile = false;

private:
	std::string draggedFile;
};