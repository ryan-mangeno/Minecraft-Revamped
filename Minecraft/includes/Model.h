#ifndef MODEL_H
#define MODEL_H

#include "Shader.h"

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb/stb_image.h>

namespace ModelLoader {

GLint texture_from_file(const char *fname, const std::string &directory);

class Model {
public:
  explicit Model(const std::string &fname);

  void init();
  void render(Shader *shader);

private:
  std::vector<ModelLoader::Mesh> meshes;
  std::vector<ModelLoader::Texture> textures;
  std::vector<ModelLoader::Texture> textures_loaded;

  std::string file_path;
  std::string directory;

  void load_model(const std::string &path);

  void process_node(aiNode *node, const aiScene *scene);

  ModelLoader::Mesh process_mesh(aiMesh *mesh, const aiScene *scene);

  std::vector<ModelLoader::Texture>
  load_material_textures(aiMaterial *mat, aiTextureType type,
                       const std::string &type_name, const aiScene *scene);
};

} // namespace ModelLoader

#endif
