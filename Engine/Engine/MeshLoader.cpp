#include "MeshLoader.h"
#include "App.h"
#include "ComponentMesh.h"

#include <iostream>
#include <fstream>

MeshLoader::MeshLoader()
{}

MeshLoader::~MeshLoader()
{}

void MeshLoader::EnableDebugger()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
}

void MeshLoader::DisableDebugger()
{
	aiDetachAllLogStreams();
}

void MeshLoader::ImportFBX(const char* path, GameObject* root)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene == nullptr) 
	{
		LOG(LogType::LOG_ERROR, "Error loading scene %s", path);
		return;
	}

	std::string fileName = path;
	fileName = fileName.substr(fileName.find_last_of("/\\") + 1);
	fileName = fileName.substr(0, fileName.find_last_of("."));

	std::vector<Mesh*> meshes;

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		std::string customFilePath = "Library/Meshes/" + fileName + std::to_string(i) + ".mesh";

		std::ifstream customFile(customFilePath, std::ios::binary);
		Mesh* mesh = nullptr;

		if (customFile.good()) 
		{
			mesh = LoadMeshFromCustomFile(customFilePath);
		}
		else 
		{
			mesh = LoadMesh(scene->mMeshes[i], scene);
			SaveMeshToCustomFile(*mesh, customFilePath);
		}

		meshes.push_back(mesh);
	}

	LoadNode(scene->mRootNode, meshes, root, fileName.c_str());

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		app->renderer3D->mesh.push_back(meshes[i]);
	}

	aiReleaseImport(scene);
}

void MeshLoader::LoadNode(aiNode* node, std::vector<Mesh*>& meshes, GameObject* parent, const char* fileName)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		Mesh* meshPtr = meshes[node->mMeshes[i]];

		GameObject* gameObjectNode = new GameObject(node->mName.C_Str(), parent);
		ComponentMesh* componentMesh = dynamic_cast<ComponentMesh*>(gameObjectNode->AddComponent(gameObjectNode->mesh));

		componentMesh->mesh = meshPtr;

		if (!meshPtr->diffuseTexturePath.empty())
		{
			Texture* newTexture = app->renderer3D->LoadTextureImage(meshPtr->diffuseTexturePath.c_str());
			if (newTexture != nullptr)
				gameObjectNode->material->AddTexture(newTexture);
		}

		parent->children.push_back(gameObjectNode);
	}

	if (node->mNumChildren > 0)
	{
		GameObject* holder = new GameObject(fileName, parent);
		parent->children.push_back(holder);

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			LoadNode(node->mChildren[i], meshes, holder, node->mName.C_Str());
		}
	}
}

Mesh* MeshLoader::LoadMesh(aiMesh* newMesh, const aiScene* scene)
{
	Mesh* mesh = new Mesh();

	// Vertices
	mesh->verticesCount = newMesh->mNumVertices;
	mesh->vertices = new float[mesh->verticesCount * 3];
	memcpy(mesh->vertices, newMesh->mVertices, sizeof(float) * mesh->verticesCount * 3);
	LOG(LogType::LOG_INFO, "New mesh with %d vertices", mesh->verticesCount);

	// Normals
	if (newMesh->HasNormals())
	{
		mesh->normalsCount = mesh->verticesCount;

		mesh->normals = new float[mesh->normalsCount * 3];
		memcpy(mesh->normals, newMesh->mNormals, sizeof(float) * mesh->normalsCount * 3);
		LOG(LogType::LOG_INFO, "New mesh with %d normals", mesh->normalsCount);
	}

	// Faces
	if (newMesh->HasFaces())
	{
		mesh->indicesCount = newMesh->mNumFaces * 3;
		mesh->indices = new uint[mesh->indicesCount];
		for (uint i = 0; i < newMesh->mNumFaces; ++i)
		{
			if (newMesh->mFaces[i].mNumIndices != 3)
			{
				LOG(LogType::LOG_WARNING, "Geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&mesh->indices[i * 3], newMesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}
		}
	}

	// Texture Coords
	if (newMesh->HasTextureCoords(0))
	{
		mesh->texCoordsCount = newMesh->mNumVertices;
		mesh->texCoords = new float[newMesh->mNumVertices * 2];

		for (unsigned int i = 0; i < mesh->texCoordsCount; i++)
		{
			mesh->texCoords[i * 2] = newMesh->mTextureCoords[0][i].x;
			mesh->texCoords[i * 2 + 1] = newMesh->mTextureCoords[0][i].y;
		}

		LOG(LogType::LOG_INFO, "New mesh with %d texture coords", mesh->texCoordsCount);
	}

	// Load Material
	if (newMesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[newMesh->mMaterialIndex];
		aiColor4D color;

		// Diffuse color
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color))
		{
			mesh->diffuseColor = glm::vec4(color.r, color.g, color.b, color.a);
			LOG(LogType::LOG_INFO, "Loaded diffuse color");
		}

		// Specular color
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color))
		{
			mesh->specularColor = glm::vec4(color.r, color.g, color.b, color.a);
			LOG(LogType::LOG_INFO, "Loaded specular color");
		}

		// Ambient color
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &color))
		{
			mesh->ambientColor = glm::vec4(color.r, color.g, color.b, color.a);
			LOG(LogType::LOG_INFO, "Loaded ambient color");
		}

		// Diffuse texture
		aiString texturePath;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
		{
			std::string basePath = "Assets/Textures/";
			mesh->diffuseTexturePath = basePath + texturePath.C_Str();
			LOG(LogType::LOG_INFO, "Loaded diffuse texture: %s", texturePath.C_Str());
		}
	}

	mesh->InitMesh();

	return mesh;
}

void MeshLoader::SaveMeshToCustomFile(const Mesh& mesh, const std::string& filePath)
{
	if (!mesh.indices || !mesh.vertices || !mesh.normals || !mesh.texCoords ||
		mesh.indicesCount == 0 || mesh.verticesCount == 0 ||
		mesh.normalsCount == 0 || mesh.texCoordsCount == 0) 
	{
		LOG(LogType::LOG_ERROR, "Invalid mesh data");
		return;
	}

	const uint32_t ranges[4] = 
	{
		mesh.indicesCount,
		mesh.verticesCount,
		mesh.normalsCount,
		mesh.texCoordsCount
	};

	// Calculate total size needed
	const size_t size = sizeof(ranges)
		+ (sizeof(uint32_t) * mesh.indicesCount)
		+ (sizeof(float) * mesh.verticesCount * 3)
		+ (sizeof(float) * mesh.normalsCount * 3)
		+ (sizeof(float) * mesh.texCoordsCount * 2)
		+ (sizeof(glm::vec4) * 3)
		+ sizeof(uint32_t)
		+ mesh.diffuseTexturePath.size() + 1;

	// Allocate buffer
	std::unique_ptr<char[]> fileBuffer(new char[size]);
	char* cursor = fileBuffer.get();

	size_t remainingSize = size;
	size_t bytes;

	// Ranges
	bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;
	remainingSize -= bytes;

	// Indices
	bytes = sizeof(uint32_t) * mesh.indicesCount;
	memcpy(cursor, mesh.indices, bytes);
	cursor += bytes;
	remainingSize -= bytes;

	// Vertices
	bytes = sizeof(float) * mesh.verticesCount * 3;
	memcpy(cursor, mesh.vertices, bytes);
	cursor += bytes;
	remainingSize -= bytes;

	// Normals
	bytes = sizeof(float) * mesh.normalsCount * 3;
	memcpy(cursor, mesh.normals, bytes);
	cursor += bytes;
	remainingSize -= bytes;

	// Texture coords
	bytes = sizeof(float) * mesh.texCoordsCount * 2;
	memcpy(cursor, mesh.texCoords, bytes);
	cursor += bytes;
	remainingSize -= bytes;

	// Materials
	bytes = sizeof(glm::vec4);
	memcpy(cursor, &mesh.diffuseColor, bytes);
	cursor += bytes;
	remainingSize -= bytes;

	memcpy(cursor, &mesh.specularColor, bytes);
	cursor += bytes;
	remainingSize -= bytes;

	memcpy(cursor, &mesh.ambientColor, bytes);
	cursor += bytes;
	remainingSize -= bytes;

	// Texture path length
	const uint32_t texturePathLength = static_cast<uint32_t>(mesh.diffuseTexturePath.size());
	bytes = sizeof(uint32_t);
	memcpy(cursor, &texturePathLength, bytes);
	cursor += bytes;
	remainingSize -= bytes;

	// Texture path
	bytes = texturePathLength + 1;
	memcpy(cursor, mesh.diffuseTexturePath.c_str(), bytes);
	cursor += bytes;

	// Write to file
	std::ofstream file(filePath, std::ios::binary);
	if (file.is_open()) 
	{
		file.write(fileBuffer.get(), size);
		file.close();
		LOG(LogType::LOG_INFO, "Mesh saved to custom file: %s", filePath.c_str());
	}
	else 
	{
		LOG(LogType::LOG_ERROR, "Failed to save mesh to custom file: %s", filePath.c_str());
	}
}

Mesh* MeshLoader::LoadMeshFromCustomFile(const std::string& filePath)
{
	std::ifstream file(filePath, std::ios::binary);
	if (!file.is_open()) 
	{
		LOG(LogType::LOG_ERROR, "Failed to open file for loading data.");
		return nullptr;
	}

	uint32_t ranges[4];
	file.read(reinterpret_cast<char*>(ranges), sizeof(ranges));

	Mesh* mesh = new Mesh();
	mesh->indicesCount = ranges[0];
	mesh->verticesCount = ranges[1];
	mesh->normalsCount = ranges[2];
	mesh->texCoordsCount = ranges[3];

	// Indices
	mesh->indices = new uint32_t[mesh->indicesCount];
	file.read(reinterpret_cast<char*>(mesh->indices), sizeof(uint32_t) * mesh->indicesCount);

	// Vertices
	mesh->vertices = new float[mesh->verticesCount * 3];
	file.read(reinterpret_cast<char*>(mesh->vertices), sizeof(float) * mesh->verticesCount * 3);

	// Normals
	mesh->normals = new float[mesh->normalsCount * 3];
	file.read(reinterpret_cast<char*>(mesh->normals), sizeof(float) * mesh->normalsCount * 3);

	// Texture coords
	mesh->texCoords = new float[mesh->texCoordsCount * 2];
	file.read(reinterpret_cast<char*>(mesh->texCoords), sizeof(float) * mesh->texCoordsCount * 2);

	// Materials
	file.read(reinterpret_cast<char*>(&mesh->diffuseColor), sizeof(glm::vec4));
	file.read(reinterpret_cast<char*>(&mesh->specularColor), sizeof(glm::vec4));
	file.read(reinterpret_cast<char*>(&mesh->ambientColor), sizeof(glm::vec4));

	// Texture
	uint32_t texturePathLength;
	file.read(reinterpret_cast<char*>(&texturePathLength), sizeof(uint32_t));
	mesh->diffuseTexturePath.resize(texturePathLength);
	file.read(&mesh->diffuseTexturePath[0], texturePathLength);

	file.close();

	mesh->InitMesh();

	return mesh;
}