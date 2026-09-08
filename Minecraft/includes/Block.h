#pragma once

using uchar = unsigned char;

struct Block
{
public:
	uchar top_min_x, top_min_y, top_max_x, top_max_y;
	uchar bottom_min_x, bottom_min_y, bottom_max_x, bottom_max_y;
	uchar side_min_x, side_min_y, side_max_x, side_max_y;

	Block(uchar min_x, uchar min_y, uchar max_x, uchar max_y);

	Block(uchar min_x, uchar min_y, uchar max_x, uchar max_y, uchar sprite_size);

	Block(uchar top_min_x, uchar top_min_y, uchar top_max_x, uchar top_max_y,
		uchar bottom_min_x, uchar bottom_min_y, uchar bottom_max_x, uchar bottom_max_y,
		uchar side_min_x, uchar side_min_y, uchar side_max_x, uchar side_max_y);

	Block(uchar top_min_x, uchar top_min_y, uchar top_max_x, uchar top_max_y,
		uchar bottom_min_x, uchar bottom_min_y, uchar bottom_max_x, uchar bottom_max_y,
		uchar side_min_x, uchar side_min_y, uchar side_max_x, uchar side_max_y, uchar sprite_size);
};
