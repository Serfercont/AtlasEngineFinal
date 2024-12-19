#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"

GameObject::GameObject(const char* name, GameObject* parent) : parent(parent), name(name)
{

	transform = new ComponentTransform(this);
	mesh = new ComponentMesh(this);
	material = new ComponentMaterial(this);

	AddComponent(transform);
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
	if (isActive)
	{
		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		{
			(*it)->Update();
		}
		for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
		{
			(*it)->Update();
		}
	}
}

void GameObject::Enable()
{
}
void GameObject::Disable()
{
}

Component* GameObject::AddComponent(Component* component)
{
	components.push_back(component);

	return component;
}

Component* GameObject::GetComponent(ComponentType type)
{
	for (auto it = components.begin(); it != components.end(); ++it) {
		if ((*it)->type == type) {
			return (*it);
		}
	}

	return nullptr;
}

AABB GameObject::GetAABB() const {
	if (mesh && mesh->mesh) { 
		return mesh->mesh->CalculateAABB(transform->globalTransform);
	}
	return AABB(glm::vec3(0.0f), glm::vec3(0.0f)); 
}
