#pragma once

#include "AABB.h"
#include "Blocks.h"
#include "World.h"
#include <glm/glm.hpp>

struct ColliderResult {
  glm::vec3 normal;
  glm::vec3 contactPoint;
  float overlapY;
  float overlapXZ;

  ColliderResult(const glm::vec3 &normal, const glm::vec3 contact,
                 float overlapY, float overlapXZ)
      : normal(normal), contactPoint(contact), overlapY(overlapY),
        overlapXZ(overlapXZ) {}
};

std::vector<glm::vec3> BroadPhase(const glm::vec3 &minPos,
                                  const glm::vec3 &maxPos);
std::vector<ColliderResult> NarrowPhase(const std::vector<glm::vec3> &blocks,
                                        glm::vec3 &playerPosition,
                                        const AABB &playerCollider);
