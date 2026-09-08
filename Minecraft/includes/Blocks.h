#pragma once

#include <array>
#include "Block.h"

constexpr uchar sprite_size = 16;



namespace Blocks
{

	enum BLOCKS : unsigned int
	{
		AIR = 0,
		DIRT,
		GRASS,
		STONE,
		SAND,
		SPRUCE_BLOCK,
		WATER
	};

	const std::array<Block, 7> blocks{
		Block(0, 0, 0, 0), // air
		Block(0, 0, 1, 1, sprite_size), // dirt

		Block(1, 1, 2, 2,  // grass
			  0, 0, 1, 1,
			  1, 0, 2, 1, sprite_size),

		Block(0, 1, 1, 2, sprite_size), // stone

		Block(4, 0, 5, 1, sprite_size), // Sand

		Block(2, 0, 3, 1, sprite_size), // wood;

		Block(0, 4, 1, 5, sprite_size) // water
	};

}
