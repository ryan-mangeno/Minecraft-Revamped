
#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>
#include "Debug.h"
#include "util.h"

class EBO
{
private:
	// ID reference of Elements Buffer Object
	GLuint m_id;


public:

	template <typename T>
	EBO(const T indices[], GLsizei size)
	{
		glGenBuffers(1, &m_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
		GlCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW));
	}

	//void load_buffer(indexCoords* indices, GLuint numIndices);

	EBO();


	// Deletes the EBO
	~EBO();

	// Binds the EBO
	void bind() const;
	// Unbinds the EBO
	void unbind() const;

	inline GLuint get_id() { return m_id; };

};

#endif
