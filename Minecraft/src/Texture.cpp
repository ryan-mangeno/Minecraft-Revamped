#include "Texture.h"
#include "Debug.h"

#include <glad/glad.h>

std::array<Texture, Texture::MAX_TEXTURE_INDEX> Texture::m_textures;

// @param numSpritesVert the number of sprites on a vertical column so when we
// do height/numSpritesVert it will give sprite size
Texture::Texture(const std::string &path, int num_sprites_vert, int format)
    : m_id(0), m_file_path(path), m_img_bytes(nullptr), m_width(0), m_height(0),
      m_bpp(0), m_sprite_size(0) {

  stbi_set_flip_vertically_on_load(true);
  m_img_bytes = stbi_load(path.c_str(), &m_width, &m_height, &m_bpp, 0);

  GLenum data_format = GL_RGBA;
  switch (m_bpp) {
  case 1:
    data_format = GL_RED;
    break;
  case 2:
    data_format = GL_RG;
    break;
  case 3:
    data_format = GL_RGB;
    break;
  case 4:
    data_format = GL_RGBA;
    break;
  default:
    std::cerr << "Failed to load texture: " << path << '\n';
    return;
  }

  GlCall(glGenTextures(1, &m_id));
  GlCall(glBindTexture(GL_TEXTURE_2D, m_id));

  GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

  GlCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
  GlCall(glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0,
                      data_format, GL_UNSIGNED_BYTE, m_img_bytes));
  GlCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
  glGenerateMipmap(GL_TEXTURE_2D);

  // GlCall(glBindTexture(GL_TEXTURE_2D, 0));

  m_sprite_size = m_height / num_sprites_vert;

  if (m_img_bytes)
    stbi_image_free(m_img_bytes);
}

Texture &Texture::get_texture(int index) { return m_textures[index]; }

void Texture::init_textures() {
  m_textures[block_atlas_index] =
      Texture((get_resource_path() / "textures/block_map.png").string(), 16,
              GL_SRGB8_ALPHA8);
  m_textures[block_atlas_normal_index] =
      Texture((get_resource_path() / "textures/block_map_normal.png").string(),
              16, GL_RGBA8);
  m_textures[block_albedo_index] =
      Texture((get_resource_path() / "textures/block_albedo.png").string(), 16,
              GL_SRGB8_ALPHA8);
  m_textures[block_ao_index] = Texture(
      (get_resource_path() / "textures/block_ao.png").string(), 16, GL_R8);
  m_textures[block_roughness_index] =
      Texture((get_resource_path() / "textures/block_roughness.png").string(),
              16, GL_R8);
  m_textures[block_metallic_index] =
      Texture((get_resource_path() / "textures/block_metallic.png").string(),
              16, GL_R8);
}

void Texture::bind(GLuint slot /*= 0*/) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture::delete_textures() {
  for (auto &tex : m_textures) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &tex.get_texture_id());
  }
}
