#pragma once

#include <glm/glm.hpp>

class AABB {
public:
    AABB(const glm::vec3& min, const glm::vec3& max);
    ~AABB();

    bool Intersects(const AABB& other) const;
    glm::vec3 GetPositiveVertex(const glm::vec3& normal) const;
    glm::vec3 GetNegativeVertex(const glm::vec3& normal) const;

private:
    glm::vec3 min;
    glm::vec3 max;
};
