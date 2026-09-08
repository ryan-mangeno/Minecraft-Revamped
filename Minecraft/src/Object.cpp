#include "Object.h"


std::vector<GLuint> Object::m_vbos;
std::vector<GLuint> Object::m_vaos;
std::vector<GLuint> Object::m_ebos;

Object::Object()
{

}


void Object::destroy_objects()
{
	for (auto& it : m_vaos)
	{
		glDeleteVertexArrays(1, &it);
	}
}
