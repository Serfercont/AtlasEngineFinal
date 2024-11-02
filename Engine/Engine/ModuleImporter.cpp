#include "ModuleImporter.h"

#include "IL/il.h"

ModuleImporter::ModuleImporter(App* app) : Module(app)
{
}

ModuleImporter::~ModuleImporter()
{
}

bool ModuleImporter::Awake()
{
	icons.folderIcon = LoadTexture("Assets/Icons/folder.png");
	icons.openFolderIcon = LoadTexture("Assets/Icons/open_folder.png");
	icons.fileIcon = LoadTexture("Assets/Icons/file.png");
	icons.infoIcon = LoadTexture("Assets/Icons/info.png");
	icons.warningIcon = LoadTexture("Assets/Icons/warning.png");
	icons.errorIcon = LoadTexture("Assets/Icons/error.png");

	return true;
}

bool ModuleImporter::CleanUp()
{
	glDeleteTextures(1, &icons.folderIcon);
	glDeleteTextures(1, &icons.openFolderIcon);
	glDeleteTextures(1, &icons.fileIcon);
	glDeleteTextures(1, &icons.infoIcon);
	glDeleteTextures(1, &icons.warningIcon);
	glDeleteTextures(1, &icons.errorIcon);

	return true;
}

GLuint ModuleImporter::LoadTexture(const std::string& filePath)
{
    ilClearColour(255, 255, 255, 255);

    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    if (ilLoadImage(filePath.c_str()) == IL_FALSE)
    {
        ilDeleteImages(1, &imageID);
        return 0;
    }

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
        ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    ilDeleteImages(1, &imageID);

    return textureID;
}