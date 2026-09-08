#pragma once

#include <glm/glm.hpp>

class AABB {
public:

	AABB(const glm::vec3& collider_min_corner_pos, float width, float height);

	bool collides(const glm::vec3& point) const;
	bool collides(float x, float y, float z) const;
	bool collides(const AABB& collider) const;

private:

	// positions for the box collider
	float min_x, min_y, min_z;
	float max_x, max_y, max_z;

};
