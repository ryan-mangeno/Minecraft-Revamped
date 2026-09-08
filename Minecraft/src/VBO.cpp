#include"VBO.h"
#include <vector>


VBO::VBO()
{
	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_id);

	// fix
	//GlCall(glBufferData(GL_ARRAY_BUFFER, sizeof(BaseBlock) * chunk_size * chunk_size * MAX_LEVELS / 2, nullptr, GL_DYNAMIC_DRAW));
}


void VBO::load_buffer(std::vector<Vertex>& vertices, GLuint num_verts)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	GlCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * num_verts, &vertices));

}

// Deletes the VBO
VBO::~VBO()
{
	glDeleteBuffers(1, &m_id);
}

// Binds the VBO
void VBO::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

// Unbinds the VBO
void VBO::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
