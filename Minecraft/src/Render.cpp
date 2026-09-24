#include "Render.h"
#include "Debug.h"
#include "World.h"
#include "render_defines.h"

bool Renderer::init(int width, int height) {
  if (!m_hdr.init(width, height)) {
    return false;
  }

  init_quad();
  m_hdr_shader = Shader::get_shader("hdr_shader");

  return true;
}

void Renderer::init_quad() {
  // x, y position followed by u, v texture coordinates
  constexpr float quad_vertices[] = {
      -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f,  1.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f,
  };

  m_quad_vao.bind();
  m_quad_vbo.bind();
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void *>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  m_quad_vao.unbind();
  m_quad_vbo.unbind();
}

void Renderer::render_quad() const {
  m_quad_vao.bind();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  m_quad_vao.unbind();
}

std::vector<DebugTexture>
Renderer::get_debug_textures(const World &world) const {
  return {
      {"Shadow map", world.get_shadow_map_texture_id(), SHADOW_MAP_WIDTH,
       SHADOW_MAP_HEIGHT},
      {"HDR scene (raw)", m_hdr.tex_id(), m_hdr.width(), m_hdr.height()},
  };
}

void Renderer::render(World &world, Shader *terrain_shader,
                      Shader *model_shader, int output_width,
                      int output_height) {
  world.render_shadow_pass();

  m_hdr.bind();
  glViewport(0, 0, m_hdr.width(), m_hdr.height());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  world.render_scene(terrain_shader, model_shader);

  HDR::unbind();
  present_hdr(output_width, output_height);
}

void Renderer::present_hdr(int output_width, int output_height) const {
  // The scene is finished, so direct subsequent output to the window.
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, output_width, output_height);

  // The quad is a screen-space carrier for the HDR texture. It should not be
  // rejected by world-space depth testing or face culling.
  const GLboolean depth_test_was_enabled = glIsEnabled(GL_DEPTH_TEST);
  const GLboolean cull_face_was_enabled = glIsEnabled(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  m_hdr_shader->bind();

  glActiveTexture(GL_TEXTURE0 + HDR_TEXTURE_SLOT);
  glBindTexture(GL_TEXTURE_2D, m_hdr.tex_id());

  render_quad();
  m_hdr_shader->unbind();

  if (depth_test_was_enabled) {
    glEnable(GL_DEPTH_TEST);
  }
  if (cull_face_was_enabled) {
    glEnable(GL_CULL_FACE);
  }
}

void Renderer::clear() const {
  // Specify the color of the background
  glClearColor(0.494f, 0.753f, 0.933f, 0.0f);
  // Clean the back buffer and assign the new color to it
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw(const VAO &va, Shader &shader, GLuint num_indices) {
  shader.bind();
  va.bind();

  GlCall(glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0));
}
