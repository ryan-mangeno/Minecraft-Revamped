#pragma once

#include "Shader.h"
#include "Thread.h"
#include "math_util.h"
#include "util.h"

#include <glm/glm.hpp>
#include <vector>

using uvec = std::vector<unsigned int>;

struct ChunkPosition {
  int x, y, z;

  // equality operator is required for unordered_map/set
  bool operator==(const ChunkPosition &other) const {
    return x == other.x && y == other.y && z == other.z;
  }
};

// i decided to change it from manually checking each chunk to see if they
// already existed to defining clearly the pair of direction and a pointer to
// the chunks block data
struct ChunkOffset {
  glm::vec3 offset;
  uvec *chunk;
};

class Chunk {
public:
  Chunk();

  Chunk(glm::vec3 chunk_pos, Thread::ThreadPool &tp);
  ~Chunk();

  void generate_chunk();
  void try_render(Shader *shader);
  void on_update();

  inline void set_render(bool render) { m_render = render; }
  inline bool get_ready_status() { return m_ready; }
  inline glm::vec3 &get_pos() { return m_chunk_pos; }
  inline uvec &get_data() { return m_chunk_data; }
  inline void set_dirty(bool dirt) { m_dirty = dirt; }
  inline bool is_dirty() { return m_dirty; }

  void set_block(int idx, unsigned int block_type);
  void make_chunk_data(int x, int y, int z, int chunk_size,
                     std::vector<unsigned int> *chunk_data);

private:
  void remake_chunk();
  void reset();

private:
  uvec m_chunk_data;
  glm::vec3 m_chunk_pos;
  bool m_ready;
  bool m_render;
  bool m_dirty = false;
  std::atomic<bool> m_generated;

  GLuint m_vao, m_vbo, m_ebo;

  unsigned int m_num_triangles;
  glm::vec3 m_world_pos;

  std::vector<Vertex> m_vertices;
  uvec m_indices;
};
