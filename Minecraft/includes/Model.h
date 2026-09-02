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

GLint TextureFromFile(const char *fname, const std::string &directory);

class Model {
public:
  explicit Model(const std::string &fname);

  void Init();
  void Render(Shader *shader);

private:
  std::vector<ModelLoader::Mesh> meshes;
  std::vector<ModelLoader::Texture> textures;
  std::vector<ModelLoader::Texture> textures_loaded;

  std::string filePath;
  std::string directory;

  void loadModel(const std::string &path);

  void processNode(aiNode *node, const aiScene *scene);

  ModelLoader::Mesh processMesh(aiMesh *mesh, const aiScene *scene);

  std::vector<ModelLoader::Texture>
  loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                       const std::string &typeName, const aiScene *scene);
};

} // namespace ModelLoader

#endif
