#pragma once
#include <glm/glm.hpp>
#include <limits>
#include <array>
#include "AABB.h"

class OBB {

public:
	std::array<glm::vec3, 8> vertices;

	OBB() = default;
	OBB(const glm::mat4& transform, const AABB& aabb) {
		vertices = {
			glm::vec3(transform * glm::vec4(aabb.minPoint.x, aabb.minPoint.y, aabb.minPoint.z, 1.0f)),
			glm::vec3(transform * glm::vec4(aabb.minPoint.x, aabb.minPoint.y, aabb.maxPoint.z, 1.0f)),
			glm::vec3(transform * glm::vec4(aabb.minPoint.x, aabb.maxPoint.y, aabb.minPoint.z, 1.0f)),
			glm::vec3(transform * glm::vec4(aabb.minPoint.x, aabb.maxPoint.y, aabb.maxPoint.z, 1.0f)),
			glm::vec3(transform * glm::vec4(aabb.maxPoint.x, aabb.minPoint.y, aabb.minPoint.z, 1.0f)),
			glm::vec3(transform * glm::vec4(aabb.maxPoint.x, aabb.minPoint.y, aabb.maxPoint.z, 1.0f)),
			glm::vec3(transform * glm::vec4(aabb.maxPoint.x, aabb.maxPoint.y, aabb.minPoint.z, 1.0f)),
			glm::vec3(transform * glm::vec4(aabb.maxPoint.x, aabb.maxPoint.y, aabb.maxPoint.z, 1.0f))
		};
	}

};
