#include "Atmosphere.h"
#include "Camera.h"
#include "Log.h"
#include "render_defines.h"

Atmosphere::~Atmosphere() {
  if (m_shadow_depth_id != 0)
    glDeleteTextures(1, &m_shadow_depth_id);
}

void Atmosphere::init() {

  m_shadow_map.init();

  // TODO: clean up depth texture created in some cleaup func
  m_shadow_depth_id = create_depth_texture(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
  m_shadow_map.attach_texture_2d(GL_DEPTH_ATTACHMENT, m_shadow_depth_id);

  m_shadow_map.set_draw_buffer(GL_NONE);
  m_shadow_map.set_read_buffer(GL_NONE);

  if (!m_shadow_map.is_complete()) {
    MC_ERROR("Atmosphere ShadowMap creation failed!");
  }

  FBO::unbind();
}

void Atmosphere::cleanup() { glDeleteTextures(1, &m_shadow_depth_id); }

void Atmosphere::begin_shadow_pass() {
  glGetIntegerv(GL_VIEWPORT, &m_prev_viewport.x);
  m_shadow_map.bind();
  glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT);
}

void Atmosphere::end_shadow_pass() {
  FBO::unbind();
  glViewport(m_prev_viewport.x, m_prev_viewport.y, m_prev_viewport.width,
             m_prev_viewport.height);
}

void Atmosphere::bind_shadow_map() const {
  glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_SLOT);
  glBindTexture(GL_TEXTURE_2D, m_shadow_depth_id);
}

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

  // move to constructor
  constexpr float shadow_half_extent = 50.0f;
  constexpr float light_distance = 100.0f;
  constexpr float near_plane = 1.0f;
  constexpr float far_plane = 200.0f;

  glm::vec3 light_pos = cam.get_pos() + (-m_sun_dir * light_distance);
  constexpr glm::vec3 sun_up_dir{0.0f, 0.0f, -1.0f};
  glm::mat4 light_view_mat = glm::lookAt(light_pos, cam.get_pos(), sun_up_dir);
  glm::mat4 light_projection_mat =
      glm::ortho(-shadow_half_extent, shadow_half_extent, -shadow_half_extent,
                 shadow_half_extent, near_plane, far_plane);
  m_light_space_matrix = light_projection_mat * light_view_mat;

  Shader *depth_shader = Shader::get_shader("depth_shader");
  depth_shader->bind();
  depth_shader->set_uniform_mat4f("uLightSpaceMatrix", m_light_space_matrix);

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
