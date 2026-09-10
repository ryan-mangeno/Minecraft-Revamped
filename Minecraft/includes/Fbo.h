#ifndef FBO_H
#define FBO_H

#include <glad/glad.h>

// Creates storage suitable for a 2D shadow map. The caller owns the returned
// texture and must eventually delete it with glDeleteTextures.
GLuint create_depth_texture(GLsizei width, GLsizei height);

struct Viewport {
  int x, y, width, height;
};

class FBO {
public:
  FBO() = default;
  ~FBO();

  FBO(const FBO &) = delete;
  FBO &operator=(const FBO &) = delete;

  FBO(FBO &&other) noexcept;
  FBO &operator=(FBO &&other) noexcept;

  void init();

  void bind(GLenum target = GL_FRAMEBUFFER) const;
  static void unbind(GLenum target = GL_FRAMEBUFFER);

  void attach_texture_2d(GLenum attachment, GLuint texture_id,
                         GLint mip_level = 0,
                         GLenum texture_target = GL_TEXTURE_2D) const;
  void attach_renderbuffer(GLenum attachment, GLuint renderbuffer_id) const;

  void set_draw_buffer(GLenum buffer) const;
  void set_read_buffer(GLenum buffer) const;

  bool is_complete(GLenum target = GL_FRAMEBUFFER) const;
  uint32_t get_id() const;

private:
  uint32_t m_id{0};
};

#endif
