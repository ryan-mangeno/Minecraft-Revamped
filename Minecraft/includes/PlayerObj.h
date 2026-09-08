#ifndef PLAYER_H
#define PLAYER_H

#include "AABB.h"
#include "Model.h"
#include <glm/glm.hpp>

class PlayerObj {

public:
  PlayerObj();
  PlayerObj(const glm::vec3 &pos);

  static ModelLoader::Model m_player_model;

protected:
  glm::vec3 m_position;
  glm::vec3 m_orientation;

  // might remove since bounding box is based off of player width and height
  AABB m_collider;

  float m_rotation;
};

#endif
