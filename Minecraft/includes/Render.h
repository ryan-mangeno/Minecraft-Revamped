#ifndef RENDERER_H
#define RENDERER_H

#include "Hdr.h"
#include "DebugTexture.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include <vector>

class World;

class Renderer {

public:
  bool init(int width, int height);
  void cleanup();

  void render(World &world, Shader *terrain_shader, Shader *model_shader,
              int output_width, int output_height);
  void render_quad() const;
  std::vector<DebugTexture> get_debug_textures(const World &world) const;

  void draw(const VAO &va, Shader &shader, GLuint num_indices);
  void clear() const;

private:
  void init_quad();
  void present_hdr(int output_width, int output_height) const;

  Shader *m_hdr_shader;

  HDR m_hdr;
  VAO m_quad_vao;
  VBO m_quad_vbo;
};

#endif
