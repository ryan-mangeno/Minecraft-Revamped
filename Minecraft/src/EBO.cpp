#include "EBO.h"


// to fix
//
//
EBO::EBO()
{
	//
	// 	glGenBuffers(1, &m_id);
	// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	// 	GlCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BaseBlockIndices) * chunk_size * chunk_size * MAX_LEVELS /2 , nullptr , GL_DYNAMIC_DRAW));
	//
}

//
// void EBO::load_buffer(indexCoords* indices, GLuint numIndices)
// {
// // 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
// // 	GlCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indexCoords) * numIndices , indices));
//
// }

// Deletes the EBO
EBO::~EBO()
{
	glDeleteBuffers(1, &m_id);
}

// Binds the EBO
void EBO::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

// Unbinds the EBO
void EBO::unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
