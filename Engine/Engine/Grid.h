#pragma once

#include "glm/glm.hpp"

using namespace glm;

class Grid
{
public:
	Grid();
	void Render();
public:
	vec3 normal = vec3(0, 1, 0);

	float lineColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float cellSize = 1.0f;
	float gridSize = 200.0f;	
	float lineWidth = 1.0f;
};