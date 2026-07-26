#ifndef PLAYER_H
#define PLAYER_H

#include "AABB.h"
#include "Model.h"
#include <glm/glm.hpp>

class PlayerObj {

public:
  PlayerObj();
  PlayerObj(const glm::vec3 &pos);

  static ModelLoader::Model m_PlayerModel;

protected:
  glm::vec3 m_Position;
  glm::vec3 m_Orientation;

  // might remove since bounding box is based off of player width and height
  AABB m_Collider;

  float m_Rotation;
};

#endif
