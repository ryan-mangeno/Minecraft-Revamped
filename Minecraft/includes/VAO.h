#ifndef VAO_CLASS_H
#define VAO_CLASS_H

class VBO;

#include <glad/glad.h>

class VAO {
public:
  // Constructor that generates a VAO ID
  VAO();
  ~VAO();

  // Links a VBO to the VAO using a certain layout
  void link_attrib(VBO &vbo, GLuint layout, GLuint num_components, GLenum type,
                   GLsizeiptr stride, void *offset);
  // Binds the VAO
  void bind() const;
  // Unbinds the VAO
  void unbind() const;

  inline uint32_t get_id() const { return m_id; };

private:
  uint32_t m_id;
};

#endif
