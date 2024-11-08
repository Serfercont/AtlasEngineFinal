#pragma once

#include "Mesh.h"
#include "GameObject.h"

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>

class MeshLoader
{
public:
	MeshLoader();
	~MeshLoader();

	void EnableDebugger();
	void DisableDebugger();

	void ImportFBX(const char* path, GameObject* gameObject);
	void LoadNode(aiNode* node, std::vector<Mesh*>& meshes, GameObject* parent, const char* fileName);
	Mesh* LoadMesh(aiMesh* newMesh, const aiScene* scene);
	void SaveMeshToCustomFile(const Mesh& mesh, const std::string& filePath);
	Mesh* LoadMeshFromCustomFile(const std::string& filePath);
};

