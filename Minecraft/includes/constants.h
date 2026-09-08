#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "Block.h"

#include <array>
#include <filesystem>
#include <glm/glm.hpp>

constexpr int max_height = 128;
constexpr int chunk_width = 16;
constexpr int chunk_length = 16;
constexpr int chunk_height = 16;
constexpr int chunk_size = 16;
constexpr int water_level = 16;
constexpr int screen_width = 1280;
constexpr int screen_height = 720;

constexpr float player_width = 0.5f;
constexpr float player_height = 1.8f;

#endif
