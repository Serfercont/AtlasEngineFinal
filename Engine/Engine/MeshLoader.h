#pragma once

#include "Mesh.h"
#include "GameObject.h"

#include <vector>

using namespace std;

class aiMesh;
class aiNode;
class aiScene;

class MeshLoader
{
public:
	MeshLoader();
	~MeshLoader();

	void EnableDebugger();
	void DisableDebugger();

	void ImportFBX(const char* path, GameObject* gameObject);
	void LoadNode(aiNode* node, vector<Mesh*>& meshes, GameObject* parent, const char* fileName);
	Mesh* LoadMesh(aiMesh* newMesh, const aiScene* scene);
};

