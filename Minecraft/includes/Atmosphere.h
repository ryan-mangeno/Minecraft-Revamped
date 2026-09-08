#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#include "Fbo.h"
#include "Shader.h"

#include <array>

struct PointLights {
  static constexpr std::size_t max_point_lights = 16;

  size_t count{0};

  std::array<glm::vec3, max_point_lights> positions;
  std::array<glm::vec3, max_point_lights> colors;
};

class Atmosphere {
public:
  Atmosphere() = default;
  Atmosphere(const glm::vec3 &sun_dir, float ambient)
      : m_sun_dir(sun_dir), m_ambient_strength(ambient) {}

  void add_light(const glm::vec3 &point, const glm::vec3 &color);

  void update(Shader *shader, float dt);
  void render(Shader *shader);

  const glm::vec3 &get_sun_direction() const { return m_sun_dir; }
  const PointLights &get_point_lights() const { return m_point_lights; }

private:
  // direction in which all light rays point, updates over dt
  glm::vec3 m_sun_dir{0.0f, -1.0f, 0.0f};
  float m_sun_speed{0.2f};
  float m_sun_theta{0.0f};

  float m_ambient_strength{0.5f};
  float m_specular_strength{1.0f};
  float m_shininess{32.f};

  FBO m_ShadowMap;

  PointLights m_point_lights;
};

#endif
