#include "Block.h"



Block::Block(uchar min_x, uchar min_y, uchar max_x, uchar max_y)
{
	top_min_x = min_x;
	top_min_y = min_y;
	top_max_x = max_x;
	top_max_y = max_y;

	bottom_min_x = min_x;
	bottom_min_y = min_y;
	bottom_max_x = max_x;
	bottom_max_y = max_y;

	side_min_x = min_x;
	side_min_y = min_y;
	side_max_x = max_x;
	side_max_y = max_y;
}

Block::Block(uchar min_x, uchar min_y, uchar max_x, uchar max_y, uchar sprite_size)
{
	top_min_x = min_x * sprite_size;
	top_min_y = min_y * sprite_size;
	top_max_x = max_x * sprite_size;
	top_max_y = max_y * sprite_size;

	bottom_min_x = min_x * sprite_size;
	bottom_min_y = min_y * sprite_size;
	bottom_max_x = max_x * sprite_size;
	bottom_max_y = max_y * sprite_size;

	side_min_x = min_x * sprite_size;
	side_min_y = min_y * sprite_size;
	side_max_x = max_x * sprite_size;
	side_max_y = max_y * sprite_size;

}

Block::Block(uchar top_min_x, uchar top_min_y, uchar top_max_x, uchar top_max_y,
	uchar bottom_min_x, uchar bottom_min_y, uchar bottom_max_x, uchar bottom_max_y,
	uchar side_min_x, uchar side_min_y, uchar side_max_x, uchar side_max_y)
{
	this->top_min_x = top_min_x;
	this->top_min_y = top_min_y;
	this->top_max_x = top_max_x;
	this->top_max_y = top_max_y;

	this->bottom_min_x = bottom_min_x;
	this->bottom_min_y = bottom_min_y;
	this->bottom_max_x = bottom_max_x;
	this->bottom_max_y = bottom_max_y;

	this->side_min_x = side_min_x;
	this->side_min_y = side_min_y;
	this->side_max_x = side_max_x;
	this->side_max_y = side_max_y;
}

Block::Block(uchar top_min_x, uchar top_min_y, uchar top_max_x, uchar top_max_y, uchar bottom_min_x, uchar bottom_min_y, uchar bottom_max_x, uchar bottom_max_y, uchar side_min_x, uchar side_min_y, uchar side_max_x, uchar side_max_y, uchar sprite_size)
{
	this->top_min_x = top_min_x * sprite_size;
	this->top_min_y = top_min_y * sprite_size;
	this->top_max_x = top_max_x * sprite_size;
	this->top_max_y = top_max_y * sprite_size;

	this->bottom_min_x = bottom_min_x * sprite_size;
	this->bottom_min_y = bottom_min_y * sprite_size;
	this->bottom_max_x = bottom_max_x * sprite_size;
	this->bottom_max_y = bottom_max_y * sprite_size;

	this->side_min_x = side_min_x * sprite_size;
	this->side_min_y = side_min_y * sprite_size;
	this->side_max_x = side_max_x * sprite_size;
	this->side_max_y = side_max_y * sprite_size;
}
