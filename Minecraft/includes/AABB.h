#pragma once

#include <glm/glm.hpp>

class AABB {
public:

	AABB(const glm::vec3& ColliderMinCornerPos, float width, float height);

	bool collides(const glm::vec3& point) const;
	bool collides(float x, float y, float z) const;
	bool collides(const AABB& collider) const;

private:

	// positions for the box collider
	float minX, minY, minZ;
	float maxX, maxY, maxZ;

};
