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

bool AABB::RayIntersectsAABB(const Ray& ray, const AABB& aabb) {
    glm::vec3 invDir = 1.0f / ray.direction;
    glm::vec3 t0s = (aabb.minPoint - ray.origin) * invDir;
    glm::vec3 t1s = (aabb.maxPoint - ray.origin) * invDir;

    glm::vec3 tMin = glm::min(t0s, t1s);
    glm::vec3 tMax = glm::max(t0s, t1s);

    float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
    float tFar = glm::min(glm::min(tMax.x, tMax.y), tMax.z);

    return tNear <= tFar && tFar >= 0.0f;
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

bool AABB::IntersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const
{
    glm::vec3 invDir = 1.0f / rayDirection;
    glm::vec3 t0s = (minPoint - rayOrigin) * invDir;
    glm::vec3 t1s = (maxPoint - rayOrigin) * invDir;

    float tmin = glm::max(glm::max(glm::min(t0s.x, t1s.x), glm::min(t0s.y, t1s.y)), glm::min(t0s.z, t1s.z));
    float tmax = glm::min(glm::min(glm::max(t0s.x, t1s.x), glm::max(t0s.y, t1s.y)), glm::max(t0s.z, t1s.z));

    return tmax >= tmin && tmax >= 0.0f;
}


