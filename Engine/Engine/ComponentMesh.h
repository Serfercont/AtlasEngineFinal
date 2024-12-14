#pragma once
#include "AABB.h"
#include "Component.h"
#include "Mesh.h"

class Mesh;

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* gameObject);
	virtual ~ComponentMesh();

	void Update() override;
	void OnEditor() override;

	void CalculateLocalAABB();

public:
	Mesh* mesh;

	AABB localAABB;

private:
	bool showVertexNormals = false;
	bool showFaceNormals = false;
};