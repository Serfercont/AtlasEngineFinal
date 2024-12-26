#include "Octree.h"
#include <iostream>
#include <algorithm>
#include "App.h"

Octree::Octree(const AABB& limits) : root(new Node(limits, 0, this)) {}
Octree::~Octree() { Clear(); delete root; }

void Octree::Clear() {
    if (root) {
        root->objects.clear();
        for (auto* child : root->children) {
            delete child;
        }
        root->children.clear();
    }
}

void Octree::Insert(GameObject* object) { if (root) root->Insert(object); }
void Octree::Remove(GameObject* object) { if (root) root->objects.erase(std::remove(root->objects.begin(), root->objects.end(), object), root->objects.end()); }
void Octree::Intersect(std::vector<GameObject*>& results, const AABB& primitive) const { if (root) root->Intersect(results, primitive); }
void Octree::DrawDebug() const { if (root) root->DrawDebug(); }

// Node implementation
Octree::Node::Node(const AABB& limits, int level, Octree* parent)
    : bounds(limits), level(level), parent(parent), maxObjects(4), maxLevels(5) {}

Octree::Node::~Node() {
    for (auto* child : children) {
        delete child;
    }
}

void Octree::Node::Subdivide() {
    glm::vec3 center = bounds.GetCenter();
    glm::vec3 halfSize = (bounds.maxPoint - bounds.minPoint) * 0.5f;

    // Crear los 8 hijos
    children.push_back(new Node(AABB(bounds.minPoint, center), level + 1, parent));
    children.push_back(new Node(AABB({ center.x, bounds.minPoint.y, bounds.minPoint.z }, { bounds.maxPoint.x, center.y, center.z }), level + 1, parent));
    children.push_back(new Node(AABB({ bounds.minPoint.x, center.y, bounds.minPoint.z }, { center.x, bounds.maxPoint.y, center.z }), level + 1, parent));
    children.push_back(new Node(AABB({ bounds.minPoint.x, bounds.minPoint.y, center.z }, { center.x, center.y, bounds.maxPoint.z }), level + 1, parent));
    children.push_back(new Node(AABB(center, bounds.maxPoint), level + 1, parent));
    children.push_back(new Node(AABB({ center.x, center.y, bounds.minPoint.z }, { bounds.maxPoint.x, bounds.maxPoint.y, center.z }), level + 1, parent));
    children.push_back(new Node(AABB({ bounds.minPoint.x, center.y, center.z }, { center.x, bounds.maxPoint.y, bounds.maxPoint.z }), level + 1, parent));
    children.push_back(new Node(AABB({ center.x, bounds.minPoint.y, center.z }, { bounds.maxPoint.x, center.y, bounds.maxPoint.z }), level + 1, parent));
}

void Octree::Node::Insert(GameObject* object) {
    if (!bounds.Intersects(object->GetAABB())) return;

    if (objects.size() < maxObjects || level >= maxLevels) {
        objects.push_back(object);
    }
    else {
        if (IsLeaf()) Subdivide();
        for (auto* child : children) child->Insert(object);
    }
}

void Octree::Node::Intersect(std::vector<GameObject*>& results, const AABB& primitive) const {
    if (!bounds.Intersects(primitive)) {
        return; // No procesar si no intersecta
    }

    for (const auto* object : objects) {
        if (object && object->GetAABB().Intersects(primitive)) {
            results.push_back(const_cast<GameObject*>(object)); // Convierte el puntero si es necesario
        }
    }

    for (const auto* child : children) {
        child->Intersect(results, primitive);
    }
}


void Octree::Node::DrawDebug() const {
    bounds.RenderAABB(glm::mat4(1.0f));
    for (const auto* child : children) child->DrawDebug();
}

void Octree::Node::CollectIntersectingObjects(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, std::vector<GameObject*>& objects) const
{
    // Verificar si el rayo intersecta este nodo
    if (!bounds.IntersectsRay(rayOrigin, rayDirection))
        return;

    // Agregar objetos si intersectan
    for (const auto& object : this->objects)
    {
        if (object->GetAABB().IntersectsRay(rayOrigin, rayDirection))
        {
            objects.push_back(object);
        }
    }

    // Comprobar hijos recursivamente
    for (const auto* child : children)
    {
        if (child)
        {
            child->CollectIntersectingObjects(rayOrigin, rayDirection, objects);
        }
    }
}

void Octree::CollectIntersectingObjects(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, std::vector<GameObject*>& objects) const
{
    if (root)
    {
        root->CollectIntersectingObjects(rayOrigin, rayDirection, objects);
    }
}
