#pragma once

#include "Camera.h"
#include "PlayerObj.h"
#include <glm/glm.hpp>

class Self : public PlayerObj {
public:
  Self(const glm::vec3 &pos);

  void on_update(float delta_time);

private:
  // camera is singleton ... might change
  // Camera m_camera;
};
