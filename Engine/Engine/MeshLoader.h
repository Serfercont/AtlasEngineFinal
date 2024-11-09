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
	void SaveMeshToCustomFile(aiMesh* newMesh, const aiScene* scene, const std::string& filePath);
	Mesh* LoadMeshFromCustomFile(const std::string& filePath);
	void SaveModelToCustomFile(const aiScene* scene, const std::string& fileName);
	void LoadModelFromCustomFile(const std::string& filePath, GameObject* root);

private:
	void SaveNodeToBuffer(const aiNode* node, std::vector<char>& buffer, size_t& currentPos);
	void LoadNodeFromBuffer(const char* buffer, size_t& currentPos, std::vector<Mesh*>& meshes, GameObject* parent, const char* fileName);
	size_t CalculateNodeSize(const aiNode* node);
};

