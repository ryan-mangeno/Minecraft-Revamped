#ifndef TEXTURE_H
#define TEXTURE_H


#include <glad/glad.h>



#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <array>


#include <stb/stb_image.h>


#include "Shader.h"
#include "util.h"
#include "Block.h"



class Texture
{

private:

	GLuint m_ID;
	std::string m_FilePath;
	unsigned char* m_ImgBytes;
	int m_Width, m_Height, m_BPP, m_SpriteSize;
	static std::array<Texture, 4> m_Textures;

	Texture(const std::string& path, int);



public:

	Texture() = default;


	static void InitTextures();
	static void DeleteTextures();

	void Bind(GLuint slot = 0) const;
	void Unbind() const;

	inline int getWidth() const { return m_Width; }
	inline int getHeight() const { return m_Height; }
	inline int getSpriteSize() const { return m_SpriteSize; };
	static Texture& GetTexture(int index);

	inline GLuint& getTextureID() { return m_ID; };

};




#endif // TEXTURE_H
