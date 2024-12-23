#pragma once
#include <glm/glm.hpp>
#include <limits>

class AABB {
public:
    glm::vec3 minPoint;
    glm::vec3 maxPoint;

    AABB() = default;
    AABB(const glm::vec3& minPoint, const glm::vec3& maxPoint) : minPoint(minPoint), maxPoint(maxPoint) {}

    AABB ApplyTransform(const glm::mat4& transform) const;

    glm::vec3 GetCenter() const;
    bool Intersects(const AABB& other) const;
    bool IntersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const
    {
        glm::vec3 invDir = 1.0f / rayDirection;
        glm::vec3 t0s = (minPoint - rayOrigin) * invDir;
        glm::vec3 t1s = (maxPoint - rayOrigin) * invDir;

        float tmin = glm::max(glm::max(glm::min(t0s.x, t1s.x), glm::min(t0s.y, t1s.y)), glm::min(t0s.z, t1s.z));
        float tmax = glm::min(glm::min(glm::max(t0s.x, t1s.x), glm::max(t0s.y, t1s.y)), glm::max(t0s.z, t1s.z));

        return tmax >= tmin && tmax >= 0.0f;
    }
    void RenderAABB(const glm::mat4& transform) const;
};
