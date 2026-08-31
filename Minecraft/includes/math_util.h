#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <glm/glm.hpp>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coords;
};

enum Corner {
  BOTTOM_LEFT = 0,
  BOTTOM_RIGHT,
  TOP_LEFT,
  TOP_RIGHT,
};

enum Direction : int8_t { NORTH = 0, SOUTH, EAST, WEST, UP, DOWN, NONE };

constexpr glm::vec3 DIRECTION_VEC[6] = {{0, 0, 1},  {0, 0, -1}, {1, 0, 0},
                                        {-1, 0, 0}, {0, 1, 0},  {0, -1, 0}};

#endif
