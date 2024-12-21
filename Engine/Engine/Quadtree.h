#ifndef QUADTREE_H
#define QUADTREE_H
#pragma once
#include <vector>
#include "GameObject.h"
#include "AABB.h"
#include "Mesh.h"

class Quadtree {
public:
    Quadtree(const AABB& limits);
    ~Quadtree();

    void Clear();
    void Insert(GameObject* object);
    void Remove(GameObject* object);
    void Intersect(std::vector<GameObject*>& results, const AABB& primitive) const;

    void DrawDebug() const;

private:
    class Node {

        friend class Quadtree;
    public:
        Node(const AABB& limits, int level, Quadtree* parent);
        ~Node();

        void Subdivide();
        void Insert(GameObject* object);
        void Intersect(std::vector<GameObject*>& results, const AABB& primitive) const;
        void DrawDebug() const;

    private:
        AABB bounds;
        std::vector<GameObject*> objects;
        std::vector<Node*> children;
        int maxObjects;
        int maxLevels;
        int level;
        Quadtree* parent;
    };

    Node* root;
};

#endif // QUADTREE_H
