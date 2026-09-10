#include "Mesh.h"

#include <glad/glad.h>
#include <utility>

ModelLoader::Mesh::Mesh(std::vector<Vertex> vertices,
                        std::vector<GLuint> indices,
                        std::vector<Texture> textures)
    : vertices(std::move(vertices)), indices(std::move(indices)),
      textures(std::move(textures)) {
  setup_mesh();
}

ModelLoader::Mesh::~Mesh() { release(); }

ModelLoader::Mesh::Mesh(Mesh &&other) noexcept
    : vertices(std::move(other.vertices)), indices(std::move(other.indices)),
      textures(std::move(other.textures)),
      vao(std::exchange(other.vao, 0)), vbo(std::exchange(other.vbo, 0)),
      ebo(std::exchange(other.ebo, 0)) {}

ModelLoader::Mesh &ModelLoader::Mesh::operator=(Mesh &&other) noexcept {
  if (this == &other)
    return *this;

  release();
  vertices = std::move(other.vertices);
  indices = std::move(other.indices);
  textures = std::move(other.textures);
  vao = std::exchange(other.vao, 0);
  vbo = std::exchange(other.vbo, 0);
  ebo = std::exchange(other.ebo, 0);
  return *this;
}

void ModelLoader::Mesh::release() {
  if (ebo != 0)
    glDeleteBuffers(1, &ebo);
  if (vbo != 0)
    glDeleteBuffers(1, &vbo);
  if (vao != 0)
    glDeleteVertexArrays(1, &vao);

  vao = 0;
  vbo = 0;
  ebo = 0;
}

void ModelLoader::Mesh::setup_mesh() {
  // create buffers/arrays
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  // load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  // set the vertex attribute pointers
  // vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, tex_coords));

  glBindVertexArray(0);
}

void ModelLoader::Mesh::render(Shader *shader) {

  unsigned int diffuse_nr = 1;
  unsigned int specular_nr = 1;
  for (GLuint i = 0; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE1 +
                    i); // Active proper texture unit before binding
    // Retrieve texture number (the N in diffuse_textureN)
    std::string number;
    std::string name = textures[i].type;

    if (name == "texture_diffuse") {
      number = std::to_string(diffuse_nr++); // Transfer GLuint to stream
    } else if (name == "texture_specular") {
      number = std::to_string(specular_nr++); // Transfer GLuint to stream
    }

    shader->set_uniform1i(name + number, int(i) + 1);

    // And finally bind the texture
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }

  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()),
                 GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glActiveTexture(GL_TEXTURE0);
}
