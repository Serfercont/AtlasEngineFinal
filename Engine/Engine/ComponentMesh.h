#pragma once
#include "AABB.h"
#include "Component.h"
#include "Mesh.h"
#include "ComponentCamera.h"

class Mesh;

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* gameObject);
	virtual ~ComponentMesh();

	void Update() override;
	void DrawMeshes(ComponentCamera* camera);
	void OnEditor() override;
public:
	Mesh* mesh;

public:
	bool showVertexNormals = false;
	bool showFaceNormals = false;

	bool showAABB = false;
	bool showOBB = false;
};