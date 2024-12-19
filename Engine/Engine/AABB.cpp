#include "AABB.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <GL/glew.h>

AABB AABB::ApplyTransform(const glm::mat4& transform) const {
    glm::vec3 vertices[8] = {
        minPoint,
        glm::vec3(minPoint.x, minPoint.y, maxPoint.z),
        glm::vec3(minPoint.x, maxPoint.y, minPoint.z),
        glm::vec3(minPoint.x, maxPoint.y, maxPoint.z),
        glm::vec3(maxPoint.x, minPoint.y, minPoint.z),
        glm::vec3(maxPoint.x, minPoint.y, maxPoint.z),
        glm::vec3(maxPoint.x, maxPoint.y, minPoint.z),
        maxPoint
    };

    glm::vec3 transformedMin(std::numeric_limits<float>::max());
    glm::vec3 transformedMax(std::numeric_limits<float>::lowest());

    for (const auto& vertice : vertices) {
        glm::vec3 transformedVertice = glm::vec3(transform * glm::vec4(vertice, 1.0f));
        transformedMin = glm::min(transformedMin, transformedVertice);
        transformedMax = glm::max(transformedMax, transformedVertice);
    }
    return AABB(transformedMin, transformedMax);
}

glm::vec3 AABB::GetCenter() const {
    return (minPoint + maxPoint) * 0.5f;
}

bool AABB::Intersects(const AABB& other) const {
    return (minPoint.x <= other.maxPoint.x && maxPoint.x >= other.minPoint.x) &&
        (minPoint.y <= other.maxPoint.y && maxPoint.y >= other.minPoint.y) &&
        (minPoint.z <= other.maxPoint.z && maxPoint.z >= other.minPoint.z);
}


void AABB::RenderAABB(const glm::mat4& transform) const {

    glm::vec3 corners[8] = {
        minPoint,
        {minPoint.x, minPoint.y, maxPoint.z},
        {minPoint.x, maxPoint.y, minPoint.z},
        {minPoint.x, maxPoint.y, maxPoint.z},
        {maxPoint.x, minPoint.y, minPoint.z},
        {maxPoint.x, minPoint.y, maxPoint.z},
        {maxPoint.x, maxPoint.y, minPoint.z},
        maxPoint
    };

    glBegin(GL_LINES);
    // Dibuja las líneas de las aristas del AABB.
    for (int i = 0; i < 4; ++i) {
        glVertex3fv(&corners[i].x); glVertex3fv(&corners[(i + 1) % 4].x);         // Base inferior
        glVertex3fv(&corners[i + 4].x); glVertex3fv(&corners[((i + 1) % 4) + 4].x); // Base superior
        glVertex3fv(&corners[i].x); glVertex3fv(&corners[i + 4].x);               // Conexión entre bases
    }
    glEnd();
}

