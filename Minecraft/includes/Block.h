#pragma once

using uchar = unsigned char;

struct Block
{
public:
	uchar topMinX, topMinY, topMaxX, topMaxY;
	uchar bottomMinX, bottomMinY, bottomMaxX, bottomMaxY;
	uchar sideMinX, sideMinY, sideMaxX, sideMaxY;

	Block(uchar minX, uchar minY, uchar maxX, uchar maxY);

	Block(uchar minX, uchar minY, uchar maxX, uchar maxY, uchar spriteSize);

	Block(uchar topMinX, uchar topMinY, uchar topMaxX, uchar topMaxY,
		uchar bottomMinX, uchar bottomMinY, uchar bottomMaxX, uchar bottomMaxY,
		uchar sideMinX, uchar sideMinY, uchar sideMaxX, uchar sideMaxY);

	Block(uchar topMinX, uchar topMinY, uchar topMaxX, uchar topMaxY,
		uchar bottomMinX, uchar bottomMinY, uchar bottomMaxX, uchar bottomMaxY,
		uchar sideMinX, uchar sideMinY, uchar sideMaxX, uchar sideMaxY, uchar spriteSize);
};
