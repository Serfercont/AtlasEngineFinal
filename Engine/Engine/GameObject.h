#pragma once

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "AABB.h"

#include <string>
#include <vector>

class GameObject
{
public:
	GameObject(const char* name, GameObject* parent);
	virtual ~GameObject();

	void Update();

	void Enable();
	void Disable();

	Component* AddComponent(Component* component);
	Component* GetComponent(ComponentType type);


	AABB GetAABB() const;
	void Delete();

public:
	GameObject* parent;
	std::string name;

	ComponentTransform* transform;
	ComponentMesh* mesh;
	ComponentMaterial* material;

	std::vector<Component*> components;
	std::vector<GameObject*> children;

	glm::vec3 aabbMin;
	glm::vec3 aabbMax;

	bool isActive = true;
	bool isEditing = false;
	bool isStatic = false;
};