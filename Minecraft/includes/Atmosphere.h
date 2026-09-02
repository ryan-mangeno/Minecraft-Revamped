#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#include "Shader.h"
#include <vector>

class Atmosphere {
public:
  static constexpr std::size_t MAX_POINT_LIGHTS = 16;

  Atmosphere() = default;
  Atmosphere(const glm::vec3 &sun_pos, float ambient)
      : m_sun(sun_pos), m_ambient_strength(ambient) {}

  void add_light(const glm::vec3 &point);

  void update(Shader *shader);
  void render(Shader *shader);

  const glm::vec3 &get_sun_pos() const { return m_sun; }
  const std::vector<glm::vec3> &get_point_lights() const {
    return m_point_lights;
  }

private:
  glm::vec3 m_sun{0.0f, 50.0f, 0.0f};
  float m_ambient_strength{0.5f};
  float m_specular_strength{1.0f};
  float m_shininess{32.f};

  std::vector<glm::vec3> m_point_lights;
};

#endif
