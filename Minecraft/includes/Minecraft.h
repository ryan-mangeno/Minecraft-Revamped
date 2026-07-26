#ifndef MINECRAFT_H
#define MINECRAFT_H

#include <iostream>
#include <stdio.h>
#include <array>
#include <memory>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Debug.h"
#include "Render.h"
#include "Texture.h"
#include "Camera.h"
#include "Block.h"

#include "Chunk.h"



class Minecraft {


private:

	bool initGL();

public:

	void Init();
	void Run();


	Minecraft() {}
	~Minecraft();


};






#endif
