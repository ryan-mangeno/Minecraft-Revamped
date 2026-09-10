#pragma once

#include "AABB.h"

#include <glm/glm.hpp>
#include <vector>

class World;

struct ColliderResult {
  glm::vec3 normal;
  glm::vec3 contact_point;
  float overlap;
};

std::vector<glm::vec3> broad_phase(World &world, const glm::vec3 &min_pos,
                                   const glm::vec3 &max_pos);
std::vector<ColliderResult> narrow_phase(const std::vector<glm::vec3> &blocks,
                                         glm::vec3 &player_position,
                                         const AABB &player_collider);
