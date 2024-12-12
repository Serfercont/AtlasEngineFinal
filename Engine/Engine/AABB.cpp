#include "AABB.h"

AABB::AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}
AABB::~AABB() {}

bool AABB::Intersects(const AABB& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
        (min.y <= other.max.y && max.y >= other.min.y) &&
        (min.z <= other.max.z && max.z >= other.min.z);
}

glm::vec3 AABB::GetPositiveVertex(const glm::vec3& normal) const {
    return glm::vec3(
        normal.x >= 0 ? max.x : min.x,
        normal.y >= 0 ? max.y : min.y,
        normal.z >= 0 ? max.z : min.z
    );
}

glm::vec3 AABB::GetNegativeVertex(const glm::vec3& normal) const {
    return glm::vec3(
        normal.x >= 0 ? min.x : max.x,
        normal.y >= 0 ? min.y : max.y,
        normal.z >= 0 ? min.z : max.z
    );
}
