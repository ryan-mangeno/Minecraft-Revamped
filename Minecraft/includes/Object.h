#ifndef OBJECT_H
#define OBJECT_H


#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include <vector>

class Object
{
public:
	Object();

	static void destroy_objects();

protected:

	static std::vector<GLuint> m_vbos;
	static std::vector<GLuint> m_vaos;
	static std::vector<GLuint> m_ebos;
};





#endif
