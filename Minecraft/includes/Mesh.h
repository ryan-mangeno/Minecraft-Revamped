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

  void render(Shader *shader);

private:
  GLuint vao, vbo, ebo;

  void setup_mesh();
};

} // namespace ModelLoader
#endif
