#ifndef MATH_UTIL_H
#define MATH_UTIL_H

struct Vertex {
  char x, y, z;
  char tex_x, tex_y;

  Vertex(char x, char y, char z, char tex_x, char tex_y)
      : x(x), y(y), z(z), tex_x(tex_x), tex_y(tex_y) {}
};

enum Corner {
  BOTTOM_LEFT = 0,
  BOTTOM_RIGHT,
  TOP_LEFT,
  TOP_RIGHT,
};

struct Normal {
  float x, y, z;
};

constexpr Normal DIRECTION_VEC[6] = {{0, 0, 1}, {-1, 0, 0}, {0, 0, -1},
                                     {1, 0, 0}, {0, -1, 0}, {0, 1, 0}};

#endif
