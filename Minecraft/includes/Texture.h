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

	GLuint m_id;
	std::string m_file_path;
	unsigned char* m_img_bytes;
	int m_width, m_height, m_bpp, m_sprite_size;
	static std::array<Texture, 4> m_textures;

	Texture(const std::string& path, int);



public:

	Texture() = default;


	static void init_textures();
	static void delete_textures();

	void bind(GLuint slot = 0) const;
	void unbind() const;

	inline int get_width() const { return m_width; }
	inline int get_height() const { return m_height; }
	inline int get_sprite_size() const { return m_sprite_size; };
	static Texture& get_texture(int index);

	inline GLuint& get_texture_id() { return m_id; };

};




#endif // TEXTURE_H
