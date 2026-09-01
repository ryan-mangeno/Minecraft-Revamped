#include "Atmosphere.h"
#include "Camera.h"

// maybe dependency inject shader?
void Atmosphere::update(Shader *shader) {
  // TODO: update sun pos over dt

  const Camera &cam = Camera::GetCamera();

  shader->Bind();

  // these uniforms are somewhat static but can add sliders
  shader->SetUniform1f("uAmbientStrength", m_ambient_strength);
  shader->SetUniform1f("uSpecularStrength", m_specular_strength);
  shader->SetUniform1f("uShininess", m_shininess);

  shader->SetUniformVec3f("uCamPos", cam.GetPos());
}

void render(Shader *shader) {
  // no atmosphere to render yet
  // sun has no rendering but does have a pos for lighting
}
