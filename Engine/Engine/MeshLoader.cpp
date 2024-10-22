#include "MeshLoader.h"
#include "App.h"
#include "ComponentMesh.h"

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

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

void MeshLoader::ImportFBX(const char* path, vector<Mesh*>& meshes, GameObject* root)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	string fileName = path;
	fileName = fileName.substr(fileName.find_last_of("\\") + 1);
	fileName = fileName.substr(0, fileName.find_last_of("."));

	if (scene != nullptr && scene->HasMeshes())
	{
		vector<Mesh*> meshes;

		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			meshes.push_back(LoadMesh(scene->mMeshes[i]));
		}

		LoadNode(scene->mRootNode, meshes, root, fileName.c_str());

		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			app->renderer3D->mesh.push_back(meshes[i]);
		}

		aiReleaseImport(scene);
	}
	else LOG(LogType::LOG_ERROR, "Error loading scene % s", path);
}

void MeshLoader::LoadNode(aiNode* node, vector<Mesh*>& meshes, GameObject* parent, const char* fileName)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		Mesh* meshPtr = meshes[node->mMeshes[i]];

		GameObject* gameObjectNode = new GameObject(node->mName.C_Str(), parent);
		ComponentMesh* componentMesh = dynamic_cast<ComponentMesh*>(gameObjectNode->AddComponent(gameObjectNode->mesh));

		componentMesh->mesh = meshPtr;
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

Mesh* MeshLoader::LoadMesh(aiMesh* newMesh)
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

	mesh->InitMesh();

	return mesh;
}