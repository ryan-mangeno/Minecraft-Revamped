#include "Model.h"
#include "Debug.h"
#include "Log.h"
#include "math_util.h"

#include <glad/glad.h>

namespace ModelLoader {

namespace {

GLenum texture_format(int component_count) {
  if (component_count == 1)
    return GL_RED;
  if (component_count == 3)
    return GL_RGB;
  if (component_count == 4)
    return GL_RGBA;
  return 0;
}

GLuint upload_texture(const unsigned char *data, int width, int height,
                      int component_count) {
  const GLenum format = texture_format(component_count);
  if (!data || format == 0)
    return 0;

  GLuint texture_id = 0;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  return texture_id;
}

GLuint texture_from_embedded(const aiTexture *embedded) {
  if (!embedded || embedded->mHeight != 0) {
    MC_ERROR("Only compressed embedded model textures are supported for now");
    return 0;
  }

  int width = 0;
  int height = 0;
  int component_count = 0;
  unsigned char *data = stbi_load_from_memory(
      reinterpret_cast<const stbi_uc *>(embedded->pcData),
      static_cast<int>(embedded->mWidth), &width, &height, &component_count, 0);

  const GLuint texture_id =
      upload_texture(data, width, height, component_count);
  stbi_image_free(data);
  return texture_id;
}

} // namespace

GLint texture_from_file(const char *fname, const std::string &directory) {
  std::string filename(fname);
  filename = directory + '/' + filename;
  int width, height, nr_components;
  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nr_components, 0);
  GLuint texture_id = 0;
  if (data) {
    texture_id = upload_texture(data, width, height, nr_components);
    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << filename << std::endl;
    stbi_image_free(data);
  }

  return texture_id;
}

Model::Model(const std::string &fname) : file_path(fname) {}

Model::~Model() {
  for (const Texture &texture : textures_loaded) {
    if (texture.id != 0)
      glDeleteTextures(1, &texture.id);
  }
}

void Model::init() {
  load_model(file_path);
  MC_DEBUG("Loading {} model", file_path.c_str());
}

void Model::render(Shader *shader) {

  for (int i = 0; i < meshes.size(); i++) {
    meshes[i].render(shader);
  }
}

void Model::load_model(const std::string &path) {
  // read file via ASSIMP
  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(
      path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
                aiProcess_GenUVCoords | aiProcess_TransformUVCoords |
                aiProcess_FlipWindingOrder);
  // check for errors
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) // if is Not Zero
  {
    MC_ERROR("ERROR::ASSIMP:: {}", importer.GetErrorString());
    return;
  }
  // retrieve the directory path of the filepath
  directory = path.substr(0, path.find_last_of('/'));

  // process ASSIMP's root node recursively
  process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode *node, const aiScene *scene) {

  // process each mesh located at the current node
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    // the node object only contains indices to index the actual objects in the
    // scene. the scene contains all the data, node is just to keep stuff
    // organized (like relations between nodes).
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(process_mesh(mesh, scene));
  }
  // after we've processed all of the meshes (if any) we then recursively
  // process each of the children nodes
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    process_node(node->mChildren[i], scene);
  }
}

ModelLoader::Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<ModelLoader::Texture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vec;

    vec.x = mesh->mVertices[i].x;
    vec.y = mesh->mVertices[i].y;
    vec.z = mesh->mVertices[i].z;
    vertex.position = vec;

    // Normals
    if (mesh->HasNormals()) {
      vec.x = mesh->mNormals[i].x;
      vec.y = mesh->mNormals[i].y;
      vec.z = mesh->mNormals[i].z;
      vertex.normal = vec;
    } else {
      vertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    // texture coordinates
    if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
    {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.tex_coords = vec;

    } else {
      vertex.tex_coords = glm::vec2(0.f, 0.f);
    }

    vertices.push_back(vertex);
  }

  for (unsigned i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];

    for (unsigned j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

  std::vector<ModelLoader::Texture> diffuse_maps = load_material_textures(
      material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
  textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

  std::vector<ModelLoader::Texture> specular_maps = load_material_textures(
      material, aiTextureType_SPECULAR, "texture_specular", scene);
  textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

  return ModelLoader::Mesh(vertices, indices, textures);
}

std::vector<ModelLoader::Texture>
Model::load_material_textures(aiMaterial *mat, aiTextureType type,
                              const std::string &type_name,
                              const aiScene *scene) {
  std::vector<ModelLoader::Texture> textures;

  for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);

    // Check if texture was loaded before and if so, continue to next iteration:
    // skip loading a new texture
    GLboolean skip = false;
    std::cout << textures_loaded.size() << "\n";
    for (GLuint j = 0; j < textures_loaded.size(); j++) {
      if (textures_loaded[j].path == str) {
        textures.push_back(textures_loaded[j]);
        skip = true; // A texture with the same filepath has already been
                     // loaded, continue to next one. (optimization)

        break;
      }
    }

    if (!skip) { // If texture hasn't been loaded already, load it
      ModelLoader::Texture texture;

      if (const aiTexture *embedded = scene->GetEmbeddedTexture(str.C_Str()))
        texture.id = texture_from_embedded(embedded);
      else
        texture.id = texture_from_file(str.C_Str(), directory);
      texture.type = type_name;
      texture.path = str;
      textures.push_back(texture);

      textures_loaded.push_back(
          texture); // Store it as texture loaded for entire model, to ensure we
                    // won't unnecesery load duplicate textures.
    }
  }

  return textures;
}

} // namespace ModelLoader
