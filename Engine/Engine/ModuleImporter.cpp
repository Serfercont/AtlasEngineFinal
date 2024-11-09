#include "ModuleImporter.h"
#include "App.h"
#include "Logger.h"

#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"
#include <Windows.h>

#include <filesystem>
#include <fstream>

ModuleImporter::ModuleImporter(App* app) : Module(app)
{

    ilInit();
    iluInit();
    ilutInit();
}

ModuleImporter::~ModuleImporter()
{
}

bool ModuleImporter::Awake()
{
    // Project
	icons.folderIcon = LoadTexture("Engine/Icons/folder.png");
	icons.openFolderIcon = LoadTexture("Engine/Icons/open_folder.png");
	icons.fileIcon = LoadTexture("Engine/Icons/file.png");
	icons.pngFileIcon = LoadTexture("Engine/Icons/file_png.png");
	icons.ddsFileIcon = LoadTexture("Engine/Icons/file_dds.png");
	icons.fbxFileIcon = LoadTexture("Engine/Icons/file_fbx.png");
    icons.dotsIcon = LoadTexture("Engine/Icons/dots.png");

    // Console
	icons.infoIcon = LoadTexture("Engine/Icons/info.png");
	icons.warningIcon = LoadTexture("Engine/Icons/warning.png");
	icons.errorIcon = LoadTexture("Engine/Icons/error.png");

	return true;
}

bool ModuleImporter::CleanUp()
{
	glDeleteTextures(1, &icons.folderIcon);
	glDeleteTextures(1, &icons.openFolderIcon);
	glDeleteTextures(1, &icons.fileIcon);
	glDeleteTextures(1, &icons.pngFileIcon);
	glDeleteTextures(1, &icons.ddsFileIcon);
	glDeleteTextures(1, &icons.fbxFileIcon);
	glDeleteTextures(1, &icons.dotsIcon);
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

void ModuleImporter::TryImportFile()
{
    if (!draggedFile.empty())
    {
        if (app->editor->sceneWindow->IsMouseInside() || app->editor->hierarchyWindow->IsMouseInside())
            app->importer->ImportFile(draggedFile, true);
        else if (app->editor->projectWindow->IsMouseInside())
            app->importer->ImportFile(draggedFile, false);

        draggedFile.clear();
    }
	isDraggingFile = false;
}

void ModuleImporter::ImportFile(const std::string& fileDir, bool addToScene)
{
    std::string extension = app->fileSystem->GetExtension(fileDir);

	bool isValidFile = extension == "fbx" || extension == "png" || extension == "dds";

    if (!isValidFile)
    {
        LOG(LogType::LOG_WARNING, "File format not supported");
        return;
    }

    std::string newDir = app->fileSystem->CopyFileIfNotExists(fileDir);

	ResourceType resourceType = app->resources->GetResourceTypeFromExtension(extension);

    Resource* newResource = app->resources->FindResourceInLibrary(newDir, resourceType);

    if (!newResource)
        newResource = ImportFileToLibrary(newDir, resourceType);

    if (!addToScene)
		return;

    switch (resourceType)
    {
	case ResourceType::MODEL:
        app->renderer3D->modelImporter.LoadModel(newResource->GetLibraryFileDir().c_str(), app->scene->root);
		break;
	case ResourceType::TEXTURE:
        Texture* newTexture = app->renderer3D->LoadTextureImage(newResource->GetLibraryFileDir().c_str());
        if (newTexture && app->editor->selectedGameObject)
        {
            app->editor->selectedGameObject->material->AddTexture(newTexture);
        }
    }
}

void ModuleImporter::SetDraggedFile(const std::string& filePath)
{
	draggedFile = filePath;
	isDraggingFile = true;
}

Resource* ModuleImporter::ImportFileToLibrary(const std::string& fileDir, ResourceType type)
{
    Resource* resource = app->resources->CreateResource(fileDir, type);

    switch (type)
    {
	case ResourceType::MODEL:
		app->renderer3D->modelImporter.SaveModel(resource);
		break;
	case ResourceType::TEXTURE:
		SaveTextureFile(resource);
        break;
    }

	return resource;
}

void ModuleImporter::SaveTextureFile(Resource* resource)
{
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    ilLoadImage(resource->GetAssetFileDir().c_str());

    ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
    ILuint size = ilSaveL(IL_DDS, nullptr, 0);

    std::vector<ILubyte> data(size);
    if (ilSaveL(IL_DDS, data.data(), size) > 0) 
    {
        std::string filePath = resource->GetLibraryFileDir();
        std::ofstream file(filePath, std::ios::binary);
        if (file.is_open()) 
        {
            file.write(reinterpret_cast<char*>(data.data()), size);
            file.close();
        }
    }

    ilDeleteImages(1, &imageID);
}