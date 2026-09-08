#ifndef RENDERER_H
#define RENDERER_H



#include "VAO.h"
#include "EBO.h"
#include "Shader.h"

class Renderer
{

public:


	void draw(const VAO& va, Shader& shader, GLuint num_indices);
	void clear() const;


private:

};














#endif
