#pragma once
#include <vector>
#include "AABB.h"
#include "GameObject.h"

class Octree {
public:
    Octree(const AABB& limits);
    ~Octree();

    void Clear();
    void Insert(GameObject* object);
    void Remove(GameObject* object);
    void Intersect(std::vector<GameObject*>& results, const AABB& primitive) const;
    void DrawDebug() const;

private:
    class Node {
        friend class Octree;
    public:
        Node(const AABB& limits, int level, Octree* parent);
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
        Octree* parent;

        bool IsLeaf() const { return children.empty(); }
    };

    Node* root;
};
