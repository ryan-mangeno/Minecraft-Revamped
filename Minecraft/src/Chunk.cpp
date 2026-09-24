#include "Chunk.h"
#include "Blocks.h"
#include "Debug.h"
#include "Texture.h"
#include "World.h"
#include "constants.h"
#include "math_util.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <simplex/OpenSimplexNoise.hh>

using uvec = std::vector<unsigned int>;

Chunk::Chunk(glm::vec3 chunk_pos, Thread::ThreadPool &tp, World &world)
    : m_world(&world) {
  m_chunk_pos = chunk_pos;
  m_world_pos = glm::vec3(chunk_pos.x * chunk_size, chunk_pos.y * chunk_size,
                          chunk_pos.z * chunk_size);
  m_ready = false;
  m_generated = false;

  // when we generate chunk we should default it to not render since it must be
  // generated in chunk first
  m_render = false;

  tp.enqueue_task(std::bind(&Chunk::generate_chunk, this));
}

Chunk::~Chunk() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
}

void Chunk::reset() {
  std::vector<Vertex>().swap(m_vertices);
  std::vector<unsigned int>().swap(m_indices);
}

void Chunk::remake_chunk() {
  World &w = *m_world;
  reset();

  uvec north_chunk, south_chunk, east_chunk, west_chunk, up_chunk, down_chunk;

  std::vector<ChunkOffset> neighbors = {
      {glm::vec3(0, 0, -1), &north_chunk}, {glm::vec3(0, 0, 1), &south_chunk},
      {glm::vec3(1, 0, 0), &east_chunk},   {glm::vec3(-1, 0, 0), &west_chunk},
      {glm::vec3(0, 1, 0), &up_chunk},     {glm::vec3(0, -1, 0), &down_chunk},
  };

  for (auto &neighbor : neighbors) {
    glm::vec3 pos = m_chunk_pos + neighbor.offset;

    // if we reset temp chunk we must upload new chunk data to vbo and ebo
    Chunk *chunk = w.get_chunk(pos.x, pos.y, pos.z);
    if (!chunk) {
      make_chunk_data(pos.x, pos.y, pos.z, chunk_size, neighbor.chunk);
    }

    else {

      uvec &temp_chunk = chunk->get_data();

      // we only need to remake surrounding chunks if they are generated
      // we might be in the generation process when we remake a chunk
      // i might have another thread that is also looking at the chunks to see
      // if a neighboring chunk is dirtied then remake it rather than remaking
      // it every single time we break a block on the main thread
      if (temp_chunk.size() != 0) {

        *neighbor.chunk = temp_chunk;
      } else {
        make_chunk_data(pos.x, pos.y, pos.z, chunk_size, neighbor.chunk);
      }
    }
  }

  unsigned int current_vertex = 0;
  for (char x = 0; x < chunk_size; x++) {
    for (char z = 0; z < chunk_size; z++) {
      for (char y = 0; y < chunk_size; y++) {
        int index = x * chunk_size * chunk_size + z * chunk_size + y;
        if (m_chunk_data[index] == Blocks::AIR)
          continue;

        const Block *block = &Blocks::blocks[m_chunk_data[index]];

        {
          int north_block = -1;

          /*

          if we are at the far north end of the chunk, the north face will
          depend on whether or not the block in the next chunk to the north is
          solid or not the same idea will apply to other blocks that we check in
          this function

          when we check the southern block face, if the current z component is
          at chunk_size it will depend on the block in the chunk south of the
          current chunk

          */
          if (z == 0) {
            int north_index =
                x * chunk_size * chunk_size + (chunk_size - 1) * chunk_size + y;
            north_block = north_chunk[north_index];
          } else {
            int north_index =
                x * chunk_size * chunk_size + (z - 1) * chunk_size + y;
            north_block = m_chunk_data[north_index];
          }

          if (north_block == Blocks::AIR) {
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 0, z + 0), direction_vec[NORTH],
                       glm::vec2(block->side_min_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 0, z + 0), direction_vec[NORTH],
                       glm::vec2(block->side_max_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 1, z + 0), direction_vec[NORTH],
                       glm::vec2(block->side_min_x, block->side_max_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 1, z + 0), direction_vec[NORTH],
                       glm::vec2(block->side_max_x, block->side_max_y)));

            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 3);
            m_indices.push_back(current_vertex + 1);
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 2);
            m_indices.push_back(current_vertex + 3);
            current_vertex += 4;
          }
        }

        {
          int south_block = -1;
          if (z < chunk_size - 1) {
            int south_index =
                x * chunk_size * chunk_size + (z + 1) * chunk_size + y;
            south_block = m_chunk_data[south_index];
          } else {
            int south_index = x * chunk_size * chunk_size + 0 * chunk_size + y;
            south_block = south_chunk[south_index];
          }
          if (south_block == Blocks::AIR) {
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 0, z + 1), direction_vec[SOUTH],
                       glm::vec2(block->side_min_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 0, z + 1), direction_vec[SOUTH],
                       glm::vec2(block->side_max_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 1, z + 1), direction_vec[SOUTH],
                       glm::vec2(block->side_min_x, block->side_max_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 1, z + 1), direction_vec[SOUTH],
                       glm::vec2(block->side_max_x, block->side_max_y)));

            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 3);
            m_indices.push_back(current_vertex + 1);
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 2);
            m_indices.push_back(current_vertex + 3);
            current_vertex += 4;
          }
        }

        {
          int west_block = -1;
          if (x > 0) {
            int block_index =
                (x - 1) * chunk_size * chunk_size + z * chunk_size + y;
            west_block = m_chunk_data[block_index];
          } else {
            int block_index =
                (chunk_size - 1) * chunk_size * chunk_size + z * chunk_size + y;
            west_block = west_chunk[block_index];
          }
          if (west_block == Blocks::AIR) {
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 0, z + 0), direction_vec[WEST],
                       glm::vec2(block->side_min_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 0, z + 1), direction_vec[WEST],
                       glm::vec2(block->side_max_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 1, z + 0), direction_vec[WEST],
                       glm::vec2(block->side_min_x, block->side_max_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 1, z + 1), direction_vec[WEST],
                       glm::vec2(block->side_max_x, block->side_max_y)));

            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 3);
            m_indices.push_back(current_vertex + 1);
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 2);
            m_indices.push_back(current_vertex + 3);
            current_vertex += 4;
          }
        }

        {
          int east_block = -1;
          if (x < chunk_size - 1) {
            int block_index =
                (x + 1) * chunk_size * chunk_size + z * chunk_size + y;
            east_block = m_chunk_data[block_index];
          } else {
            int block_index = 0 * chunk_size * chunk_size + z * chunk_size + y;
            east_block = east_chunk[block_index];
          }
          if (east_block == Blocks::AIR) {
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 0, z + 1), direction_vec[EAST],
                       glm::vec2(block->side_min_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 0, z + 0), direction_vec[EAST],
                       glm::vec2(block->side_max_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 1, z + 1), direction_vec[EAST],
                       glm::vec2(block->side_min_x, block->side_max_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 1, z + 0), direction_vec[EAST],
                       glm::vec2(block->side_max_x, block->side_max_y)));

            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 3);
            m_indices.push_back(current_vertex + 1);
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 2);
            m_indices.push_back(current_vertex + 3);
            current_vertex += 4;
          }
        }

        {
          int bottom_block = -1;
          if (y > 0) {
            int block_index =
                x * chunk_size * chunk_size + z * chunk_size + (y - 1);
            bottom_block = m_chunk_data[block_index];
          } else {
            int block_index =
                x * chunk_size * chunk_size + z * chunk_size + (chunk_size - 1);
            bottom_block = down_chunk[block_index];
          }
          if (bottom_block == Blocks::AIR) {
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 0, z + 1), direction_vec[DOWN],
                       glm::vec2(block->bottom_min_x, block->bottom_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 0, z + 1), direction_vec[DOWN],
                       glm::vec2(block->bottom_max_x, block->bottom_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 0, z + 0), direction_vec[DOWN],
                       glm::vec2(block->bottom_min_x, block->bottom_max_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 0, z + 0), direction_vec[DOWN],
                       glm::vec2(block->bottom_max_x, block->bottom_max_y)));
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 3);
            m_indices.push_back(current_vertex + 1);
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 2);
            m_indices.push_back(current_vertex + 3);
            current_vertex += 4;
          }
        }

        {
          int top_block = -1;
          if (y < chunk_size - 1) {
            int block_index =
                x * chunk_size * chunk_size + z * chunk_size + (y + 1);
            top_block = m_chunk_data[block_index];
          } else {
            int block_index = x * chunk_size * chunk_size + z * chunk_size;
            top_block = up_chunk[block_index];
          }
          if (top_block == Blocks::AIR) {
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 1, z + 1), direction_vec[UP],
                       glm::vec2(block->top_min_x, block->top_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 1, z + 1), direction_vec[UP],
                       glm::vec2(block->top_max_x, block->top_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 1, z + 0), direction_vec[UP],
                       glm::vec2(block->top_min_x, block->top_max_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 1, z + 0), direction_vec[UP],
                       glm::vec2(block->top_max_x, block->top_max_y)));

            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 3);
            m_indices.push_back(current_vertex + 1);
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 2);
            m_indices.push_back(current_vertex + 3);
            current_vertex += 4;
          }
        }
      }
    }
  }

  m_generated.store(true);
}

void Chunk::generate_chunk() {

  World &w = *m_world;

  make_chunk_data(m_chunk_pos.x, m_chunk_pos.y, m_chunk_pos.z, chunk_size,
                  &m_chunk_data);

  uvec north_chunk, south_chunk, east_chunk, west_chunk, up_chunk, down_chunk;

  std::vector<ChunkOffset> neighbors = {
      {glm::vec3(0, 0, -1), &north_chunk}, {glm::vec3(0, 0, 1), &south_chunk},
      {glm::vec3(1, 0, 0), &east_chunk},   {glm::vec3(-1, 0, 0), &west_chunk},
      {glm::vec3(0, 1, 0), &up_chunk},     {glm::vec3(0, -1, 0), &down_chunk},
  };

  // loop through neighboring chunks, if they already were generated, then we
  // can skip else it will call make_chunk_data and populate the uvecs defined
  // above
  for (auto &neighbor : neighbors) {
    glm::vec3 pos = m_chunk_pos + neighbor.offset;
    uvec &temp_chunk = w.get_chunk_data(pos.x, pos.y, pos.z);
    if (temp_chunk.size() == 0) {
      make_chunk_data(pos.x, pos.y, pos.z, chunk_size, neighbor.chunk);
    } else {
      *neighbor.chunk = temp_chunk;
    }
  }

  unsigned int current_vertex = 0;
  for (char x = 0; x < chunk_size; x++) {
    for (char z = 0; z < chunk_size; z++) {
      for (char y = 0; y < chunk_size; y++) {
        int index = x * chunk_size * chunk_size + z * chunk_size + y;
        if (m_chunk_data[index] == Blocks::AIR)
          continue;

        const Block *block = &Blocks::blocks[m_chunk_data[index]];

        {
          int north_block = -1;

          /*

          if we are at the far north end of the chunk, the north face will
          depend on whether or not the block in the next chunk to the north is
          solid or not the same idea will apply to other blocks that we check in
          this function

          when we check the southern block face, if the current z component is
          at chunk_size it will depend on the block in the chunk south of the
          current chunk

          */
          if (z == 0) {
            int north_index =
                x * chunk_size * chunk_size + (chunk_size - 1) * chunk_size + y;
            north_block = north_chunk[north_index];
          } else {
            int north_index =
                x * chunk_size * chunk_size + (z - 1) * chunk_size + y;
            north_block = m_chunk_data[north_index];
          }

          if (north_block == Blocks::AIR) {
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 0, z + 0), direction_vec[NORTH],
                       glm::vec2(block->side_min_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 0, z + 0), direction_vec[NORTH],
                       glm::vec2(block->side_max_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 1, z + 0), direction_vec[NORTH],
                       glm::vec2(block->side_min_x, block->side_max_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 1, z + 0), direction_vec[NORTH],
                       glm::vec2(block->side_max_x, block->side_max_y)));

            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 3);
            m_indices.push_back(current_vertex + 1);
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 2);
            m_indices.push_back(current_vertex + 3);
            current_vertex += 4;
          }
        }

        {
          int south_block = -1;
          if (z < chunk_size - 1) {
            int south_index =
                x * chunk_size * chunk_size + (z + 1) * chunk_size + y;
            south_block = m_chunk_data[south_index];
          } else {
            int south_index = x * chunk_size * chunk_size + 0 * chunk_size + y;
            south_block = south_chunk[south_index];
          }
          if (south_block == Blocks::AIR) {
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 0, z + 1), direction_vec[SOUTH],
                       glm::vec2(block->side_min_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 0, z + 1), direction_vec[SOUTH],
                       glm::vec2(block->side_max_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 1, z + 1), direction_vec[SOUTH],
                       glm::vec2(block->side_min_x, block->side_max_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 1, z + 1), direction_vec[SOUTH],
                       glm::vec2(block->side_max_x, block->side_max_y)));

            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 3);
            m_indices.push_back(current_vertex + 1);
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 2);
            m_indices.push_back(current_vertex + 3);
            current_vertex += 4;
          }
        }

        {
          int west_block = -1;
          if (x > 0) {
            int block_index =
                (x - 1) * chunk_size * chunk_size + z * chunk_size + y;
            west_block = m_chunk_data[block_index];
          } else {
            int block_index =
                (chunk_size - 1) * chunk_size * chunk_size + z * chunk_size + y;
            west_block = west_chunk[block_index];
          }
          if (west_block == Blocks::AIR) {
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 0, z + 0), direction_vec[WEST],
                       glm::vec2(block->side_min_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 0, z + 1), direction_vec[WEST],
                       glm::vec2(block->side_max_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 1, z + 0), direction_vec[WEST],
                       glm::vec2(block->side_min_x, block->side_max_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 1, z + 1), direction_vec[WEST],
                       glm::vec2(block->side_max_x, block->side_max_y)));

            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 3);
            m_indices.push_back(current_vertex + 1);
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 2);
            m_indices.push_back(current_vertex + 3);
            current_vertex += 4;
          }
        }

        {
          int east_block = -1;
          if (x < chunk_size - 1) {
            int block_index =
                (x + 1) * chunk_size * chunk_size + z * chunk_size + y;
            east_block = m_chunk_data[block_index];
          } else {
            int block_index = 0 * chunk_size * chunk_size + z * chunk_size + y;
            east_block = east_chunk[block_index];
          }
          if (east_block == Blocks::AIR) {
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 0, z + 1), direction_vec[EAST],
                       glm::vec2(block->side_min_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 0, z + 0), direction_vec[EAST],
                       glm::vec2(block->side_max_x, block->side_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 1, z + 1), direction_vec[EAST],
                       glm::vec2(block->side_min_x, block->side_max_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 1, z + 0), direction_vec[EAST],
                       glm::vec2(block->side_max_x, block->side_max_y)));

            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 3);
            m_indices.push_back(current_vertex + 1);
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 2);
            m_indices.push_back(current_vertex + 3);
            current_vertex += 4;
          }
        }

        {
          int bottom_block = -1;
          if (y > 0) {
            int block_index =
                x * chunk_size * chunk_size + z * chunk_size + (y - 1);
            bottom_block = m_chunk_data[block_index];
          } else {
            int block_index =
                x * chunk_size * chunk_size + z * chunk_size + (chunk_size - 1);
            bottom_block = down_chunk[block_index];
          }
          if (bottom_block == Blocks::AIR) {
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 0, z + 1), direction_vec[DOWN],
                       glm::vec2(block->bottom_min_x, block->bottom_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 0, z + 1), direction_vec[DOWN],
                       glm::vec2(block->bottom_max_x, block->bottom_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 0, z + 0), direction_vec[DOWN],
                       glm::vec2(block->bottom_min_x, block->bottom_max_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 0, z + 0), direction_vec[DOWN],
                       glm::vec2(block->bottom_max_x, block->bottom_max_y)));
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 3);
            m_indices.push_back(current_vertex + 1);
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 2);
            m_indices.push_back(current_vertex + 3);
            current_vertex += 4;
          }
        }

        {
          int top_block = -1;
          if (y < chunk_size - 1) {
            int block_index =
                x * chunk_size * chunk_size + z * chunk_size + (y + 1);
            top_block = m_chunk_data[block_index];
          } else {
            int block_index = x * chunk_size * chunk_size + z * chunk_size;
            top_block = up_chunk[block_index];
          }
          if (top_block == Blocks::AIR) {
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 1, z + 1), direction_vec[UP],
                       glm::vec2(block->top_min_x, block->top_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 1, z + 1), direction_vec[UP],
                       glm::vec2(block->top_max_x, block->top_min_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 0, y + 1, z + 0), direction_vec[UP],
                       glm::vec2(block->top_min_x, block->top_max_y)));
            m_vertices.push_back(
                Vertex(glm::vec3(x + 1, y + 1, z + 0), direction_vec[UP],
                       glm::vec2(block->top_max_x, block->top_max_y)));

            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 3);
            m_indices.push_back(current_vertex + 1);
            m_indices.push_back(current_vertex + 0);
            m_indices.push_back(current_vertex + 2);
            m_indices.push_back(current_vertex + 3);
            current_vertex += 4;
          }
        }
      }
    }
  }

  m_generated.store(true);
}

void Chunk::on_update() {
  if (!m_ready) {
    if (m_generated.load()) {
      m_num_triangles = m_indices.size();

      glGenVertexArrays(1, &m_vao);
      glBindVertexArray(m_vao);

      glGenBuffers(1, &m_vbo);
      glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
      glBufferData(GL_ARRAY_BUFFER,
                   chunk_size * chunk_size * chunk_size * sizeof(Vertex),
                   nullptr, GL_DYNAMIC_DRAW);

      glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(Vertex),
                      m_vertices.data());

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                            (void *)offsetof(Vertex, position));
      glEnableVertexAttribArray(0);

      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                            (void *)offsetof(Vertex, normal));
      glEnableVertexAttribArray(1);

      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                            (void *)offsetof(Vertex, tex_coords));
      glEnableVertexAttribArray(2);

      glGenBuffers(1, &m_ebo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   m_indices.size() * sizeof(unsigned int), nullptr,
                   GL_DYNAMIC_DRAW);
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                      m_indices.size() * sizeof(unsigned int),
                      m_indices.data());

      glBindVertexArray(0);

      m_ready = true;
      m_render = true;
    }
  }

  if (m_dirty) {

    remake_chunk();

    m_dirty = false;

    m_num_triangles = m_indices.size();

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(Vertex),
                    m_vertices.data());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                    m_indices.size() * sizeof(unsigned int), m_indices.data());
  }
}

void Chunk::try_render(Shader *shader) {
  if (m_render && m_ready) {
    shader->bind();

    Texture::get_texture(0).bind(0);
    shader->set_uniform1i("uAtlas", 0);

    glBindVertexArray(m_vao);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_world_pos);

    shader->set_uniform_mat4f("model", model);

    glDrawElements(GL_TRIANGLES, m_num_triangles, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
  }
}

void Chunk::set_block(int idx, unsigned int block_type) {
  m_chunk_data.at(idx) = block_type;
}

void Chunk::make_chunk_data(int chunk_x, int chunk_y, int chunk_z,
                            int chunk_size, uvec *chunk_data) {

  chunk_data->reserve(chunk_size * chunk_size * chunk_size);

  OSN::Noise<2> surface_noise;
  OSN::Noise<3> cave_noise;
  int base_x = chunk_x * chunk_size;
  int base_y = chunk_y * chunk_size;
  int base_z = chunk_z * chunk_size;

  constexpr float height_scale = 12.0f;
  constexpr float noise_scale = 0.07f;

  for (int x = 0; x < chunk_size; x++) {
    for (int z = 0; z < chunk_size; z++) {
      int noise_y = (surface_noise.eval((float)(x + base_x) * .1f,
                                        (float)(z + base_z) * .1f) *
                     height_scale) +
                    20;
      for (int y = 0; y < chunk_size; y++) {

        float noise_caves = cave_noise.eval((float)(x + base_x) * noise_scale,
                                            (float)(y + base_y) * noise_scale,
                                            (float)(z + base_z) * noise_scale);

        int num_water_blocks = noise_y - water_level;

        // water -> temp, just looks cool for now
        if (y + base_y < water_level && y + base_y > noise_y) {
          chunk_data->push_back(Blocks::WATER);
        }

        // caves
        else if (y + base_y > noise_y || noise_caves > .5f)
          chunk_data->push_back(Blocks::AIR);

        // ground and underground, sand and water stuff is temp
        else if (y + base_y == noise_y && noise_y == water_level)
          chunk_data->push_back(Blocks::SAND);

        else if (y + base_y == noise_y - num_water_blocks - 1 &&
                 noise_y < water_level)
          chunk_data->push_back(Blocks::SAND);

        else if (y + base_y == noise_y)
          chunk_data->push_back(Blocks::GRASS);

        else if (y + base_y > 10)
          chunk_data->push_back(Blocks::DIRT);

        else
          chunk_data->push_back(Blocks::STONE);
      }
    }
  }
}
