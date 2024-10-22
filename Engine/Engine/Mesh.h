#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
typedef unsigned int uint;

class Mesh
{
public:
	void InitMesh();
	void DrawMesh(GLuint textureID);
	void CleanUpMesh();

public:
	uint indicesId = 0;
	uint indicesCount = 0;
	uint* indices = nullptr;
	uint verticesId = 0;
	uint verticesCount = 0;
	float* vertices = nullptr;
	uint normalsId = 0;
	uint normalsCount = 0;
	float* normals = nullptr;
	uint texCoordsId = 0;
	uint texCoordsCount = 0;
	float* texCoords = nullptr;
};