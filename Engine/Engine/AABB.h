#pragma once
#include <glm/glm.hpp>
#include <limits>

class AABB {
public:
    glm::vec3 minPoint;
    glm::vec3 maxPoint;
 
	AABB() = default;
	AABB(const glm::vec3& minPoint, const glm::vec3& maxPoint) : minPoint(minPoint), maxPoint(maxPoint) {}

	AABB ApplyTransform(const glm::mat4& transform) const {

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
};
