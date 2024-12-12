#include "Frustum.h"
#include <glm/gtc/type_ptr.hpp>

Frustum::Frustum() {}
Frustum::~Frustum() {}

void Frustum::Calculate(const glm::mat4& viewProjectionMatrix) {
    const float* m = glm::value_ptr(viewProjectionMatrix);

    // Extract planes (example: LEFT plane)
    planes[LEFT] = glm::vec4(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12]);
    planes[RIGHT] = glm::vec4(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12]);
    // Repeat for TOP, BOTTOM, NEAR, FAR...
	planes[TOP] = glm::vec4(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13]);
	planes[BOTTOM] = glm::vec4(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13]);
	planes[NEAR] = glm::vec4(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14]);
	planes[FAR] = glm::vec4(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14]);


    // Normalize planes
    for (int i = 0; i < 6; ++i) {
        float length = glm::length(glm::vec3(planes[i]));
        planes[i] /= length;
    }
}

bool Frustum::IsBoxInside(const AABB& box) const {
    for (int i = 0; i < 6; ++i) {
        glm::vec3 normal(planes[i].x, planes[i].y, planes[i].z);
        float distance = planes[i].w;

        // Compute positive vertex (most aligned with plane normal)
        glm::vec3 p = box.GetPositiveVertex(normal);

        // Check if box is completely outside
        if (glm::dot(normal, p) + distance < 0) {
            return false;
        }
    }
    return true;
}
