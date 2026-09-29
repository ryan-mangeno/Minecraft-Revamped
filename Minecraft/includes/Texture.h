#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>

#include <stb/stb_image.h>

#include "Block.h"
#include "Shader.h"
#include "util.h"

class Texture {

public:
  static const int block_atlas_index = 0;
  static const int block_atlas_normal_index = 1;

public:
  Texture() = default;

  static void init_textures();
  static void delete_textures();

  void bind(GLuint slot = 0) const;
  void unbind() const;

  inline int get_width() const { return m_width; }
  inline int get_height() const { return m_height; }
  inline int get_sprite_size() const { return m_sprite_size; };
  static Texture &get_texture(int index);

  inline GLuint &get_texture_id() { return m_id; };

private:
  GLuint m_id;
  std::string m_file_path;
  unsigned char *m_img_bytes;
  int m_width, m_height, m_bpp, m_sprite_size;
  static std::array<Texture, 4> m_textures;

  Texture(const std::string &path, int num_sprites, int format);
};

#endif // TEXTURE_H
