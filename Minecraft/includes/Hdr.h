#ifndef HDR_H
#define HDR_H

#include "Fbo.h"

class HDR {
public:
  HDR() = default;
  ~HDR();

  HDR(const HDR &) = delete;
  HDR &operator=(const HDR &) = delete;
  HDR(HDR &&) = delete;
  HDR &operator=(HDR &&) = delete;

  bool init(int width, int height);

  void bind() const;
  static void unbind();

  inline uint32_t tex_id() const { return m_color_texture_id; }
  inline uint32_t depth_id() const { return m_depth_renderbuffer_id; }
  inline int width() const { return m_width; }
  inline int height() const { return m_height; }

private:
  void cleanup();

  FBO m_fbo;
  uint32_t m_color_texture_id{0};
  uint32_t m_depth_renderbuffer_id{0};
  int m_width{0};
  int m_height{0};
};

#endif
