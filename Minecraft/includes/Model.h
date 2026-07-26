#ifndef MODEL_H
#define MODEL_H

#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb/stb_image.h>

namespace ModelLoader {

	GLint TextureFromFile(const char* fname, const std::string& directory);


	class Model {
	public:
		Model(const char* fname);

		void Init();
		void Render(Shader* shader);

	private:
		std::vector<ModelLoader::Mesh> meshes;
		std::vector<ModelLoader::Texture> textures;
		std::vector<ModelLoader::Texture> textures_loaded;

		const char* filePath;
		std::string directory;

		void loadModel(std::string path);

		void processNode(aiNode* node, const aiScene* scene);

		ModelLoader::Mesh processMesh(aiMesh* mesh, const aiScene* scene);

		std::vector<ModelLoader::Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	};

}

#endif
