#include "Quadtree.h"
#include <iostream>
#include <algorithm>

Quadtree::Quadtree(const AABB& limits)
    : root(new Node(limits, 0, this)) {}

Quadtree::~Quadtree() {
    Clear();
    delete root;
}

void Quadtree::Clear() {
    if (root) {
        root->objects.clear();
        for (auto* child : root->children) {
            delete child;
        }
        root->children.clear();
    }
}

void Quadtree::Insert(GameObject* object) {
    if (root) {
        root->Insert(object);
    }
}

void Quadtree::Remove(GameObject* object) {
    if (root) {
        root->objects.erase(std::remove(root->objects.begin(), root->objects.end(), object), root->objects.end());
    }
}

void Quadtree::Intersect(std::vector<GameObject*>& results, const AABB& primitive) const {
    if (root) {
        root->Intersect(results, primitive);
    }
}

void Quadtree::DrawDebug() const {
    if (root) {
        root->DrawDebug();
    }
}

Quadtree::Node::Node(const AABB& limits, int level, Quadtree* parent)
    : bounds(limits), level(level), parent(parent), maxObjects(4), maxLevels(5) {}

Quadtree::Node::~Node() {
    for (auto* child : children) {
        delete child;
    }
}

void Quadtree::Node::Subdivide() {
    glm::vec3 center = bounds.GetCenter();
    glm::vec3 halfSize = (bounds.maxPoint - bounds.minPoint) * 0.5f;

    children.push_back(new Node(AABB(bounds.minPoint, center), level + 1, parent));
    children.push_back(new Node(AABB(glm::vec3(center.x, bounds.minPoint.y, bounds.minPoint.z), glm::vec3(bounds.maxPoint.x, center.y, center.z)), level + 1, parent));
    children.push_back(new Node(AABB(glm::vec3(bounds.minPoint.x, center.y, bounds.minPoint.z), glm::vec3(center.x, bounds.maxPoint.y, center.z)), level + 1, parent));
    children.push_back(new Node(AABB(center, bounds.maxPoint), level + 1, parent));
}

void Quadtree::Node::Insert(GameObject* object) {
    if (!bounds.Intersects(object->GetAABB())) {
        return;
    }

    if (objects.size() < maxObjects || level >= maxLevels) {
        objects.push_back(object);
    }
    else {
        if (children.empty()) {
            Subdivide();
        }

        for (auto* child : children) {
            child->Insert(object);
        }
    }
}

void Quadtree::Node::Intersect(std::vector<GameObject*>& results, const AABB& primitive) const {
    if (!bounds.Intersects(primitive)) {
        return; 
    }

    for (const auto* object : objects) {
        if (object && object->GetAABB().Intersects(primitive)) {
            results.push_back(const_cast<GameObject*>(object)); 
        }
    }

    for (const auto* child : children) {
        child->Intersect(results, primitive);
    }
}



void Quadtree::Node::DrawDebug() const {
    bounds.RenderAABB(glm::mat4(1.0f));

    for (const auto* child : children) {
        child->DrawDebug();
    }

    for (const auto* object : objects) {
        if (object && object->mesh && object->mesh->mesh) {
            object->mesh->mesh->RenderAABB(object->transform->globalTransform);
        }
    }
}


