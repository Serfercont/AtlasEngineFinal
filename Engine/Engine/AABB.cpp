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
    AABB transformedAABB = ApplyTransform(transform);

    glm::vec3 corners[8] = {
        {transformedAABB.minPoint.x, transformedAABB.minPoint.y, transformedAABB.minPoint.z}, 
        {transformedAABB.maxPoint.x, transformedAABB.minPoint.y, transformedAABB.minPoint.z},
        {transformedAABB.maxPoint.x, transformedAABB.minPoint.y, transformedAABB.maxPoint.z}, 
        {transformedAABB.minPoint.x, transformedAABB.minPoint.y, transformedAABB.maxPoint.z},
        {transformedAABB.minPoint.x, transformedAABB.maxPoint.y, transformedAABB.minPoint.z}, 
        {transformedAABB.maxPoint.x, transformedAABB.maxPoint.y, transformedAABB.minPoint.z}, 
        {transformedAABB.maxPoint.x, transformedAABB.maxPoint.y, transformedAABB.maxPoint.z}, 
        {transformedAABB.minPoint.x, transformedAABB.maxPoint.y, transformedAABB.maxPoint.z}  
    };

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 1.0f);

    glBegin(GL_LINES);

    
    glVertex3fv(&corners[0].x); glVertex3fv(&corners[1].x); 
    glVertex3fv(&corners[1].x); glVertex3fv(&corners[2].x); 
    glVertex3fv(&corners[2].x); glVertex3fv(&corners[3].x); 
    glVertex3fv(&corners[3].x); glVertex3fv(&corners[0].x); 

    glVertex3fv(&corners[4].x); glVertex3fv(&corners[5].x);
    glVertex3fv(&corners[5].x); glVertex3fv(&corners[6].x); 
    glVertex3fv(&corners[6].x); glVertex3fv(&corners[7].x); 
    glVertex3fv(&corners[7].x); glVertex3fv(&corners[4].x); 

    glVertex3fv(&corners[0].x); glVertex3fv(&corners[4].x); 
    glVertex3fv(&corners[1].x); glVertex3fv(&corners[5].x); 
    glVertex3fv(&corners[2].x); glVertex3fv(&corners[6].x); 
    glVertex3fv(&corners[3].x); glVertex3fv(&corners[7].x);

    glEnd();
    glPopAttrib();
}
