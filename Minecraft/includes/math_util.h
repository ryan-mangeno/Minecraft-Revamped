#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <cstdint>
#include <glm/glm.hpp>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coords;
  glm::vec3 tangent;
  glm::vec3 bitangent;

  Vertex() = default;

  Vertex(const glm::vec3 &position, const glm::vec3 &normal,
         const glm::vec2 &tex_coords)
      : position(position), normal(normal), tex_coords(tex_coords) {
    if (glm::abs(normal.x) > 0.5f) {
      tangent = glm::vec3(0.0f, 0.0f, -normal.x);
    } else if (glm::abs(normal.y) > 0.5f) {
      tangent = glm::vec3(normal.y, 0.0f, 0.0f);
    } else {
      tangent = glm::vec3(normal.z, 0.0f, 0.0f);
    }

    bitangent = glm::cross(normal, tangent);
  }
};

enum Corner {
  BOTTOM_LEFT = 0,
  BOTTOM_RIGHT,
  TOP_LEFT,
  TOP_RIGHT,
};

enum Direction : int8_t { NORTH = 0, SOUTH, EAST, WEST, UP, DOWN, NONE };

// All three arrays use Direction order: NORTH, SOUTH, EAST, WEST, UP, DOWN.
// Tangent follows increasing texture U; bitangent follows increasing texture V.
constexpr glm::vec3 direction_vec[6] = {
    {0, 0, -1}, // NORTH: z = 0
    {0, 0, 1},  // SOUTH: z = 1
    {1, 0, 0},  // EAST:  x = 1
    {-1, 0, 0}, // WEST:  x = 0
    {0, 1, 0},  // UP:    y = 1
    {0, -1, 0}, // DOWN:  y = 0
};

constexpr glm::vec3 tangent_vec[6] = {
    {-1, 0, 0}, // NORTH
    {1, 0, 0},  // SOUTH
    {0, 0, -1}, // EAST
    {0, 0, 1},  // WEST
    {1, 0, 0},  // UP
    {-1, 0, 0}, // DOWN
};

constexpr glm::vec3 bitangent_vec[6] = {
    {0, 1, 0},  // NORTH
    {0, 1, 0},  // SOUTH
    {0, 1, 0},  // EAST
    {0, 1, 0},  // WEST
    {0, 0, -1}, // UP
    {0, 0, -1}, // DOWN
};
#endif
