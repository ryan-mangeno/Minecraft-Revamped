#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include <vector>

#include "Debug.h"
#include "math_util.h"

class VBO {

public:
  template <typename T> VBO(const T vertices[], GLsizei size) {
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    GlCall(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
  }

  void load_buffer(std::vector<Vertex> &vertices, GLuint num_verts);

  VBO();

  ~VBO();

  void bind() const;
  void unbind() const;

  inline uint32_t get_id() { return m_id; };

private:
  uint32_t m_id;
};

#endif
