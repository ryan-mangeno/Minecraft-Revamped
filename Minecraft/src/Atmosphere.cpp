#include "Atmosphere.h"
#include "Camera.h"

void Atmosphere::add_light(const glm::vec3 &point) {
  if (m_point_lights.size() >= MAX_POINT_LIGHTS)
    return;

  m_point_lights.emplace_back(point);
}

// maybe dependency inject shader?
void Atmosphere::update(Shader *shader) {
  // TODO: update sun pos over dt

  const Camera &cam = Camera::GetCamera();

  shader->Bind();

  // these uniforms are somewhat static but can add sliders
  shader->SetUniform1f("uAmbientStrength", m_ambient_strength);
  shader->SetUniform1f("uSpecularStrength", m_specular_strength);
  shader->SetUniform1f("uShininess", m_shininess);

  // Tell the shader how much of the fixed-size GLSL array is valid.
  shader->SetUniform1i("uPointLightCount",
                       static_cast<int>(m_point_lights.size()));
  shader->SetUniformVec3Array("uPointLightPositions", m_point_lights);

  shader->SetUniformVec3f("uCamPos", cam.GetPos());
}

void render(Shader *shader) {
  // no atmosphere to render yet
  // sun has no rendering but does have a pos for lighting
}
