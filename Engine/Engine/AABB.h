#pragma once
#include <glm/glm.hpp>
#include <limits>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};
class AABB {
public:
    glm::vec3 minPoint;
    glm::vec3 maxPoint;

    AABB() = default;
    AABB(const glm::vec3& minPoint, const glm::vec3& maxPoint) : minPoint(minPoint), maxPoint(maxPoint) {}

    AABB ApplyTransform(const glm::mat4& transform) const;

    glm::vec3 GetCenter() const;
    bool Intersects(const AABB& other) const;
    void RenderAABB(const glm::mat4& transform) const;
};
