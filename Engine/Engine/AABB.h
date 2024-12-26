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
        glm::vec3 invDir = 1.0f / rayDirection; // Inverso de la dirección
        glm::vec3 tMin = (minPoint - rayOrigin) * invDir;
        glm::vec3 tMax = (maxPoint - rayOrigin) * invDir;

        glm::vec3 t1 = glm::min(tMin, tMax);
        glm::vec3 t2 = glm::max(tMin, tMax);

        float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
        float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

        return tNear <= tFar && tFar >= 0.0f;
    }

    void RenderAABB(const glm::mat4& transform) const;
};
