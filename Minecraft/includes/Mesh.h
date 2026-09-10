#ifndef MESH_H
#define MESH_H

#include "Shader.h"
#include "math_util.h"

#include <iostream>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ModelLoader {

struct Texture {
  GLuint id{0};
  std::string type;
  aiString path;
};

class Mesh {
public:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> textures;

  Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
       std::vector<Texture> textures);
  ~Mesh();

  Mesh(const Mesh &) = delete;
  Mesh &operator=(const Mesh &) = delete;
  Mesh(Mesh &&other) noexcept;
  Mesh &operator=(Mesh &&other) noexcept;

  void render(Shader *shader);

private:
  uint32_t vao{0};
  uint32_t vbo{0};
  uint32_t ebo{0};

  void setup_mesh();
  void release();
};

} // namespace ModelLoader
#endif
