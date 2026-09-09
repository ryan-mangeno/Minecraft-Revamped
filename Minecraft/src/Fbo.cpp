#include "Fbo.h"

#include <utility>

GLuint create_depth_texture(GLsizei width, GLsizei height) {
  GLuint texture_id = 0;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  constexpr GLfloat border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

  glBindTexture(GL_TEXTURE_2D, 0);
  return texture_id;
}

// maybe move to some defined destroy method
// to avoid destruction ordering issues
FBO::~FBO() {
  if (m_id != 0) {
    glDeleteFramebuffers(1, &m_id);
  }
}

FBO::FBO(FBO &&other) noexcept : m_id(std::exchange(other.m_id, 0)) {}

FBO &FBO::operator=(FBO &&other) noexcept {
  if (this == &other) {
    return *this;
  }

  if (m_id != 0) {
    glDeleteFramebuffers(1, &m_id);
  }

  m_id = std::exchange(other.m_id, 0);
  return *this;
}

// support multiple buffers in constructor
void FBO::init() { glGenFramebuffers(1, &m_id); }

void FBO::bind(GLenum target) const { glBindFramebuffer(target, m_id); }

void FBO::unbind(GLenum target) { glBindFramebuffer(target, 0); }

void FBO::attach_texture_2d(GLenum attachment, GLuint texture_id,
                            GLint mip_level, GLenum texture_target) const {
  bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture_target, texture_id,
                         mip_level);
}

void FBO::attach_renderbuffer(GLenum attachment, GLuint renderbuffer_id) const {
  bind();
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER,
                            renderbuffer_id);
}

void FBO::set_draw_buffer(GLenum buffer) const {
  bind();
  glDrawBuffer(buffer);
}

void FBO::set_read_buffer(GLenum buffer) const {
  bind();
  glReadBuffer(buffer);
}

bool FBO::is_complete(GLenum target) const {
  bind(target);
  return glCheckFramebufferStatus(target) == GL_FRAMEBUFFER_COMPLETE;
}

GLuint FBO::get_id() const { return m_id; }
