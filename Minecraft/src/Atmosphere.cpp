#include "Atmosphere.h"
#include "Camera.h"

// maybe dependency inject shader?
void Atmosphere::update(Shader *shader) {
  // TODO: update sun pos over dt

  const Camera &cam = Camera::GetCamera();

  shader->Bind();
  shader->SetUniform1f("uAmbient", m_ambient_strength);
  shader->SetUniform1f("uSpecular", m_specular_strength);
  shader->SetUniformVec3f("uCamPos", cam.GetPos());
}

void render(Shader *shader) {
  // no atmosphere to render yet
  // sun has no rendering but does have a pos for lighting
}
