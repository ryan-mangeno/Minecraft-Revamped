#include "AABB.h"


// this kind of aabb can be used with radius to make it act like a cylindrical collider
AABB::AABB(const glm::vec3& collider_min_corner_pos, float width, float height)
	: min_x(collider_min_corner_pos.x), min_y(collider_min_corner_pos.y), min_z(collider_min_corner_pos.z),
	  max_x(collider_min_corner_pos.x + width), max_y(collider_min_corner_pos.y + height), max_z(collider_min_corner_pos.z + width)
{
}

bool AABB::collides(const glm::vec3& point) const
{
	if(point.x >= min_x && point.x <= max_x &&
	   point.y >= min_y && point.y <= max_y &&
       point.z >= min_z && point.z <= max_z) {
		return true;
	}

	return false;
}

bool AABB::collides(float x, float y, float z) const
{
	return collides(glm::vec3(x,y,z));
}

// assumes aabb's dont rotate .. this is fine for this clone since I will not be dealing with
// swimming ( yet ... we will see )
bool AABB::collides(const AABB& collider) const
{
	// Check for overlap on all three axes
	if (max_x < collider.min_x || min_x > collider.max_x) return false;
	if (max_y < collider.min_y || min_y > collider.max_y) return false;
	if (max_z < collider.min_z || min_z > collider.max_z) return false;

	// If none of the above conditions are true, the boxes are colliding
	return true;
}
