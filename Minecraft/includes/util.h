#ifndef UTIL_H
#define UTIL_H


#include "Block.h"
#include <glm/glm.hpp>
#include <array>
#include <filesystem>>


struct Vertex
{
	char posX, posY, posZ;
	char texGridX, texGridY;

	Vertex(char _posX, char _posY, char _posZ, char _texGridX, char _texGridY)
	{
		posX = _posX;
		posY = _posY;
		posZ = _posZ;

		texGridX = _texGridX;
		texGridY = _texGridY;
	}
};

enum Corner {
	BOTTOM_LEFT = 0,
	BOTTOM_RIGHT,
	TOP_LEFT,
	TOP_RIGHT,
};


struct normal {
	float x, y, z;
};

constexpr normal DIRECTION_VEC[6] = {
	{  0,  0,  1 },
	{ -1,  0,  0 },
	{  0,  0, -1 },
	{  1,  0,  0 },
	{  0, -1,  0 },
	{  0,  1,  0 }
};



constexpr int MAX_LEVELS = 64;
constexpr int CHUNK_SIZE = 16;
constexpr int WATER_LEVEL = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;

constexpr float PLAYER_WIDTH = 0.6f;
constexpr float PLAYER_HEIGHT = 1.8f;


std::filesystem::path getExecutablePath();

#endif
