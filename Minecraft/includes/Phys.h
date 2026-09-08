#pragma once

#include "AABB.h"
#include "Blocks.h"
#include "World.h"
#include <glm/glm.hpp>

struct ColliderResult {
  glm::vec3 normal;
  glm::vec3 contact_point;
  float overlap_y;
  float overlap_xz;

  ColliderResult(const glm::vec3 &normal, const glm::vec3 contact,
                 float overlap_y, float overlap_xz)
      : normal(normal), contact_point(contact), overlap_y(overlap_y),
        overlap_xz(overlap_xz) {}
};

std::vector<glm::vec3> broad_phase(const glm::vec3 &min_pos,
                                  const glm::vec3 &max_pos);
std::vector<ColliderResult> narrow_phase(const std::vector<glm::vec3> &blocks,
                                        glm::vec3 &player_position,
                                        const AABB &player_collider);
