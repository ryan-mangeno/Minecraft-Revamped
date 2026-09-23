#include "Hdr.h"

HDR::~HDR() { cleanup(); }

bool HDR::init(int width, int height) {
  if (width <= 0 || height <= 0 || m_fbo.get_id() != 0) {
    return false;
  }

  m_width = width;
  m_height = height;
  m_fbo.init();

  glGenTextures(1, &m_color_texture_id);
  glBindTexture(GL_TEXTURE_2D, m_color_texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
               GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);
  m_fbo.attach_texture_2d(GL_COLOR_ATTACHMENT0, m_color_texture_id);

  glGenRenderbuffers(1, &m_depth_renderbuffer_id);
  glBindRenderbuffer(GL_RENDERBUFFER, m_depth_renderbuffer_id);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  m_fbo.attach_renderbuffer(GL_DEPTH_ATTACHMENT, m_depth_renderbuffer_id);

  m_fbo.set_draw_buffer(GL_COLOR_ATTACHMENT0);
  const bool complete = m_fbo.is_complete();
  FBO::unbind();
  return complete;
}

void HDR::bind() const { m_fbo.bind(); }

void HDR::unbind() { FBO::unbind(); }

void HDR::cleanup() {
  if (m_depth_renderbuffer_id != 0) {
    glDeleteRenderbuffers(1, &m_depth_renderbuffer_id);
    m_depth_renderbuffer_id = 0;
  }

  if (m_color_texture_id != 0) {
    glDeleteTextures(1, &m_color_texture_id);
    m_color_texture_id = 0;
  }

  m_width = 0;
  m_height = 0;
}
