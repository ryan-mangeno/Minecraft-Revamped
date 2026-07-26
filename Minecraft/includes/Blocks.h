#pragma once

#include <array>
#include "Block.h"

constexpr uchar spriteSize = 16;



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
		Block(0, 0, 1, 1, spriteSize), // dirt

		Block(1, 1, 2, 2,  // grass
			  0, 0, 1, 1,
			  1, 0, 2, 1, spriteSize),

		Block(0, 1, 1, 2, spriteSize), // stone

		Block(4, 0, 5, 1, spriteSize), // Sand

		Block(2, 0, 3, 1, spriteSize), // wood;

		Block(0, 4, 1, 5, spriteSize) // water
	};

}
