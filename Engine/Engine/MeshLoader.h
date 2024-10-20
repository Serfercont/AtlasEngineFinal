#pragma once

#include "Mesh.h"
#include <vector>

class MeshLoader
{
public:
	MeshLoader();
	~MeshLoader();

	void EnableDebugger();
	void DisableDebugger();

	void ImportFBX(const char* path, std::vector<Mesh*>& meshes, int textureId);
};

