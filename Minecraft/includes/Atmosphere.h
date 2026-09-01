#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#include "Shader.h"

class Atmosphere {
public:
  // Atmosphere(const glm::vec3& sun_pos, float ambient);

  void update(Shader *shader);
  void render(Shader *shader);

  const glm::vec3 &get_sun_pos() const { return m_sun; }

private:
  glm::vec3 m_sun{0.0f, 50.0f, 0.0f};
  float m_ambient_strength{0.5f};
  float m_specular_strength{32.f};
};

#endif
