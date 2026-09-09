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

  // maybe pass in shadow map params but will default for now
  void init();
  void begin_shadow_pass();
  void end_shadow_pass();
  void bind_shadow_map() const;

  void add_light(const glm::vec3 &point, const glm::vec3 &color);

  void update(Shader *shader, float dt);
  void render(Shader *shader);

  const glm::vec3 &get_sun_direction() const { return m_sun_dir; }
  const glm::mat4 &get_light_space_matrix() const {
    return m_light_space_matrix;
  }
  const PointLights &get_point_lights() const { return m_point_lights; }

private:
  // direction in which all light rays point, updates over dt
  glm::vec3 m_sun_dir{0.0f, -1.0f, 0.0f};
  float m_sun_speed{0.2f};
  float m_sun_theta{0.0f};

  float m_ambient_strength{0.5f};
  float m_specular_strength{1.0f};
  float m_shininess{32.f};

  FBO m_shadow_map;
  uint32_t m_shadow_depth_id{0};
  glm::mat4 m_light_space_matrix{1.0f};
  GLint m_previous_viewport[4]{0, 0, 0, 0};

  PointLights m_point_lights;
};

#endif
