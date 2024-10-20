#include "MeshLoader.h"
#include "App.h"

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

void MeshLoader::ImportFBX(const char* path, std::vector<Mesh*>& meshes, int textureId)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (!scene) {
		fprintf(stderr, "Error en carregar el fitxer: %s\n", aiGetErrorString());
	}

	if (scene != nullptr && scene->HasMeshes())
	{
		aiMesh* newMesh = nullptr;
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			newMesh = scene->mMeshes[i];
			Mesh* mesh = new Mesh();
			// Vertices
			mesh->verticesCount = newMesh->mNumVertices;
			mesh->vertices = new float[mesh->verticesCount * 3];
			memcpy(mesh->vertices, newMesh->mVertices, sizeof(float) * mesh->verticesCount * 3);
			// Faces
			if (newMesh->HasNormals())
			{
				mesh->normalsCount = mesh->verticesCount;
				mesh->normals = new float[mesh->normalsCount * 3];
				memcpy(mesh->normals, newMesh->mNormals, sizeof(float) * mesh->normalsCount * 3);
			}
			if (newMesh->HasFaces())
			{
				mesh->indicesCount = newMesh->mNumFaces * 3;
				mesh->indices = new uint[mesh->indicesCount];
				for (uint i = 0; i < newMesh->mNumFaces; ++i)
				{
					if (newMesh->mFaces[i].mNumIndices != 3)
					{
					}
					else
					{
						memcpy(&mesh->indices[i * 3], newMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			if (newMesh->HasTextureCoords(0))
			{
				mesh->texCoordsCount = newMesh->mNumVertices;
				mesh->texCoords = new float[newMesh->mNumVertices * 2];
				for (unsigned int i = 0; i < mesh->texCoordsCount; i++)
				{
					mesh->texCoords[i * 2] = newMesh->mTextureCoords[0][i].x;
					mesh->texCoords[i * 2 + 1] = newMesh->mTextureCoords[0][i].y;
				}
				mesh->textureID = textureId;
			}

			mesh->InitMesh();
			meshes.push_back(mesh);
		}
		aiReleaseImport(scene);
	}
}