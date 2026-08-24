#include "AABB.h"


// this kind of aabb can be used with radius to make it act like a cylindrical collider
AABB::AABB(const glm::vec3& ColliderMinCornerPos, float width, float height)
	: minX(ColliderMinCornerPos.x), minY(ColliderMinCornerPos.y), minZ(ColliderMinCornerPos.z),
	  maxX(ColliderMinCornerPos.x + width), maxY(ColliderMinCornerPos.y + height), maxZ(ColliderMinCornerPos.z + width)
{
}

bool AABB::collides(const glm::vec3& point) const
{
	if(point.x >= minX && point.x <= maxX &&
	   point.y >= minY && point.y <= maxY &&
       point.z >= minZ && point.z <= maxZ) {
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
	if (maxX < collider.minX || minX > collider.maxX) return false;
	if (maxY < collider.minY || minY > collider.maxY) return false;
	if (maxZ < collider.minZ || minZ > collider.maxZ) return false;

	// If none of the above conditions are true, the boxes are colliding
	return true;
}
