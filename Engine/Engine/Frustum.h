#pragma once

#include <glm/glm.hpp>
#include "AABB.h"

class Frustum {
public:
    enum Plane {
        NEAR = 0, FAR, LEFT, RIGHT, TOP, BOTTOM
    };

    Frustum();
    ~Frustum();

    void Calculate(const glm::mat4& viewProjectionMatrix);
    bool IsBoxInside(const AABB& box) const;

private:
    glm::vec4 planes[6]; // Each plane represented as Ax + By + Cz + D = 0
};
