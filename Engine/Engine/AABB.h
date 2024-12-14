// AABB.h
#pragma once
#include <glm/glm.hpp>
#include <limits>

class AABB {
public:
    glm::vec3 minPoint;
    glm::vec3 maxPoint;

    AABB() {
        SetNegativeInfinity();
    }

    void SetNegativeInfinity() {
        minPoint = glm::vec3(std::numeric_limits<float>::infinity());
        maxPoint = glm::vec3(-std::numeric_limits<float>::infinity());
    }

    void Enclose(const glm::vec3& point) {
        minPoint = glm::min(minPoint, point);
        maxPoint = glm::max(maxPoint, point);
    }

    glm::vec3 Center() const {
        return (minPoint + maxPoint) * 0.5f;
    }

    glm::vec3 Size() const {
        return maxPoint - minPoint;
    }
};
