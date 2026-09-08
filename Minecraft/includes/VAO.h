#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"
#include "Debug.h"
#include "util.h"

class VAO
{
	GLuint m_id;
public:

	// Constructor that generates a VAO ID
	VAO();
	~VAO();

	// Links a VBO to the VAO using a certain layout
	void link_attrib(VBO& vbo, GLuint layout, GLuint num_components, GLenum type, GLsizeiptr stride, void* offset );
	// Binds the VAO
	void bind() const;
	// Unbinds the VAO
	void unbind() const;

	inline GLuint get_id() const { return m_id; };

};
#endif
