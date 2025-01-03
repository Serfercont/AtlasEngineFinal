#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "App.h"

GameObject::GameObject(const char* name, GameObject* parent) : parent(parent), name(name)
{

	transform = new ComponentTransform(this);
	mesh = new ComponentMesh(this);
	material = new ComponentMaterial(this);
	camera = nullptr;

	AddComponent(transform);
}

GameObject::~GameObject()
{
	for (auto component : components) {
		delete component;
	}
	components.clear();

	for (auto child : children) {
		delete child;
	}
	children.clear();

	mesh = nullptr; 
	transform = nullptr;
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
	if (mesh != nullptr && mesh->mesh != nullptr) {
		return mesh->mesh->CalculateAABB(transform->globalTransform);
	}
	return AABB(glm::vec3(0.0f), glm::vec3(0.0f));
	
}

void GameObject::Delete() {
	isDeleted = true; 

	if (app->scene->octreeScene) {
		app->scene->octreeScene->Remove(this);
	}

	if (parent) {
		auto& siblings = parent->children;
		siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
	}

	for (auto child : children) {
		child->Delete();
		delete child;
	}
	children.clear();

	for (auto component : components) {
		delete component;
	}
	components.clear();

	parent = nullptr;
}


bool GameObject::IntersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& intersectionDistance) const
{
	if (!transform || !mesh || !mesh->mesh)
		return false;

	const glm::mat4& transformMatrix = transform->globalTransform;
	const float* vertices = mesh->mesh->vertices;
	const uint* indices = mesh->mesh->indices;
	uint indicesCount = mesh->mesh->indicesCount;
	uint verticesCount = mesh->mesh->verticesCount;

	bool hit = false;
	float closestDistance = FLT_MAX;

	for (uint i = 0; i < indicesCount; i += 3)
	{
		if (indices[i] >= verticesCount || indices[i + 1] >= verticesCount || indices[i + 2] >= verticesCount)
			return false;

		glm::vec3 v0 = glm::vec3(transformMatrix * glm::vec4(vertices[indices[i] * 3], vertices[indices[i] * 3 + 1], vertices[indices[i] * 3 + 2], 1.0f));
		glm::vec3 v1 = glm::vec3(transformMatrix * glm::vec4(vertices[indices[i + 1] * 3], vertices[indices[i + 1] * 3 + 1], vertices[indices[i + 1] * 3 + 2], 1.0f));
		glm::vec3 v2 = glm::vec3(transformMatrix * glm::vec4(vertices[indices[i + 2] * 3], vertices[indices[i + 2] * 3 + 1], vertices[indices[i + 2] * 3 + 2], 1.0f));

		glm::vec3 edge1 = v1 - v0;
		glm::vec3 edge2 = v2 - v0;
		glm::vec3 h = glm::cross(rayDirection, edge2);
		float a = glm::dot(edge1, h);

		if (fabs(a) < 0.0001f)
			continue;

		float f = 1.0f / a;
		glm::vec3 s = rayOrigin - v0;
		float u = f * glm::dot(s, h);

		if (u < 0.0f || u > 1.0f)
			continue;

		glm::vec3 q = glm::cross(s, edge1);
		float v = f * glm::dot(rayDirection, q);

		if (v < 0.0f || u + v > 1.0f)
			continue;

		float t = f * glm::dot(edge2, q);

		if (t > 0.0001f && t < closestDistance)
		{
			closestDistance = t;
			hit = true;
		}
	}

	if (hit)
	{
		intersectionDistance = closestDistance;
		return true;
	}

	return false;
}
