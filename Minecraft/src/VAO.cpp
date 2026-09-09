#include "VAO.h"
#include "VBO.h"

// Constructor that generates a VAO ID
VAO::VAO() {
  glGenVertexArrays(1, &m_id);
  bind();
}

VAO::~VAO() { glDeleteVertexArrays(1, &m_id); }

// Links a VBO to the VAO using a certain layout
void VAO::link_attrib(VBO &vbo, GLuint layout, GLuint num_components,
                      GLenum type, GLsizeiptr stride, void *offset) {
  vbo.bind();
  glVertexAttribPointer(layout, num_components, type, GL_FALSE, stride, offset);
  glEnableVertexAttribArray(layout);
  vbo.unbind();
}

// Binds the VAO
void VAO::bind() const { glBindVertexArray(m_id); }

// Unbinds the VAO
void VAO::unbind() const { glBindVertexArray(0); }
