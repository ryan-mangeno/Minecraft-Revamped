#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#include "Shader.h"

class Atmosphere {
public:
  void update();
  void render(const Shader &shader);

  const glm::vec3 &get_sun_pos() const { return m_sun; }

private:
  glm::vec3 m_sun{0.0f, 50.0f, 0.0f};
};

#endif
