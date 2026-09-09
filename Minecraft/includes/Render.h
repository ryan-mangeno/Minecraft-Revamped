#ifndef RENDERER_H
#define RENDERER_H

#include "Shader.h"
#include "VAO.h"

class Renderer {

public:
  void draw(const VAO &va, Shader &shader, GLuint num_indices);
  void clear() const;

private:
};

#endif
