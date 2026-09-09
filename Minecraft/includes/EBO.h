
#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include "Debug.h"

class EBO {

public:
  template <typename T> EBO(const T indices[], GLsizei size) {
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    GlCall(
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW));
  }

  // void load_buffer(indexCoords* indices, GLuint numIndices);

  EBO();

  // Deletes the EBO
  ~EBO();

  // Binds the EBO
  void bind() const;
  // Unbinds the EBO
  void unbind() const;

  inline uint32_t get_id() { return m_id; };

private:
  uint32_t m_id;
};

#endif
