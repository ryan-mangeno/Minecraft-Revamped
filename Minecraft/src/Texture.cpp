#include "Texture.h"
#include "Debug.h"

#include <glad/glad.h>

std::array<Texture, 4> Texture::m_Textures;


// @param numSpritesVert the number of sprites on a vertical column so when we do height/numSpritesVert it will give sprite size
Texture::Texture(const std::string& path, int numSpritesVert)
	: m_ID(0), m_FilePath(path), m_ImgBytes(nullptr), m_Width(0), m_BPP(0), m_Height(0)
{

	stbi_set_flip_vertically_on_load(true);
	m_ImgBytes = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 0);

	GlCall(glGenTextures(1, &m_ID));
	GlCall(glBindTexture(GL_TEXTURE_2D, m_ID));

	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GlCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));


	GlCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_ImgBytes));
	glGenerateMipmap(GL_TEXTURE_2D);

	//GlCall(glBindTexture(GL_TEXTURE_2D, 0));

	m_SpriteSize = m_Height / numSpritesVert;


	if (m_ImgBytes)
		stbi_image_free(m_ImgBytes);


}


Texture& Texture::GetTexture(int index) {
	return m_Textures[index];
}

void Texture::InitTextures()
{
	m_Textures[0] = Texture(
    (getResourcePath() / "textures/block_map.png").string(),
    16
);
}


void Texture::Bind(GLuint slot /*= 0*/) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::DeleteTextures()
{
	for (auto& tex : m_Textures)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &tex.getTextureID());
	}
}
