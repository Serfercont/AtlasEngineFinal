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
	// Liberar componentes
	for (auto component : components)
	{
		delete component;
	}
	components.clear();

	// Liberar hijos
	for (auto child : children)
	{
		delete child;
	}
	children.clear();
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

void GameObject::Delete()
{
	
	if (parent)
	{
		auto& siblings = parent->children;
		siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
	}

	
	for (auto child : children)
	{
		child->Delete();
		delete child;
	}
	children.clear();

	
	for (auto component : components)
	{
		delete component;
	}
	components.clear();
}
