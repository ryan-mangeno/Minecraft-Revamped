#include "Atmosphere.h"
#include "Camera.h"

void Atmosphere::add_light(const glm::vec3 &point, const glm::vec3 &color) {
  if (m_point_lights.count >= PointLights::max_point_lights)
    return;

  const size_t idx = m_point_lights.count++;
  m_point_lights.positions[idx] = point;
  m_point_lights.colors[idx] = color;
}

// maybe dependency inject shader?
void Atmosphere::update(Shader *shader, float dt) {

  m_sun_theta += dt * m_sun_speed;

  constexpr float full_circle = glm::two_pi<float>();

  if (m_sun_theta > full_circle) {
    m_sun_theta -= full_circle;
  }

  m_sun_dir = glm::vec3(glm::cos(m_sun_theta), -glm::sin(m_sun_theta), 0.0f);

  const Camera &cam = Camera::get_camera();

  shader->bind();

  // these uniforms are somewhat static but can add sliders
  shader->set_uniform1f("uAmbientStrength", m_ambient_strength);
  shader->set_uniform1f("uSpecularStrength", m_specular_strength);
  shader->set_uniform1f("uShininess", m_shininess);

  // Tell the shader how much of the fixed-size GLSL array is valid.
  shader->set_uniform1i("uPointLightCount",
                       static_cast<int>(m_point_lights.count));
  shader->set_uniform_vec3_array("uPointLightPositions",
                              &m_point_lights.positions.data()->x,
                              m_point_lights.count);
  shader->set_uniform_vec3_array("uPointLightColors",
                              &m_point_lights.colors.data()->r,
                              m_point_lights.count);

  shader->set_uniform_vec3f("uCamPos", cam.get_pos());
}

void render(Shader *shader) {
  // no atmosphere to render yet
  // sun has no rendering but does have a pos for lighting
}
