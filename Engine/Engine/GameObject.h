#pragma once

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "AABB.h"
#include "ComponentCamera.h"

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

	bool IntersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& intersectionDistance) const;

	


public:
	GameObject* parent;
	std::string name;

	ComponentTransform* transform;
	ComponentMesh* mesh;
	ComponentMaterial* material;
	ComponentCamera* camera;

	std::vector<Component*> components;
	std::vector<GameObject*> children;

	glm::vec3 aabbMin;
	glm::vec3 aabbMax;

	bool isActive = true;
	bool isEditing = false;
	bool isStatic = false;
	bool isDeleted = false;
};