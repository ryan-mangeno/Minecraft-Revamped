#pragma once

#include <glm/glm.hpp>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "Atmosphere.h"
#include "Chunk.h"
#include "Hash.h"
#include "Model.h"
#include "Shader.h"
#include "util.h"

using cTuple = std::tuple<int, int, int>;

class World {

public:
  World();
  ~World();

  std::vector<unsigned int> &get_chunk_data(int chunk_x, int chunk_y, int chunk_z);
  void update(glm::vec3 cam_pos, Shader *shader, float dt);
  void render_shadow_pass();
  void render_scene(Shader *terrain_shader, Shader *model_shader);
  uint32_t get_shadow_map_texture_id() const {
    return m_atmosphere.get_shadow_depth_id();
  }

  inline void set_dirty_chunk(int chunk_x, int chunk_y, int chunk_z) {
    cTuple chunk_tuple{chunk_x, chunk_y, chunk_z};
    auto chunk = m_chunks.find(chunk_tuple);
    if (chunk != m_chunks.end())
      chunk->second.set_dirty(true);
  }

  inline Chunk *get_chunk(int chunk_x, int chunk_y, int chunk_z) {
    cTuple chunk_tuple{chunk_x, chunk_y, chunk_z};
    auto chunk = m_chunks.find(chunk_tuple);
    return chunk != m_chunks.end() ? &chunk->second : nullptr;
  }

  void mark_neighbors(int lx, int ly, int lz, int cx, int cy, int cz);

private:
  std::unordered_map<cTuple, Chunk> m_chunks;
  std::queue<glm::vec3> m_queue;

  // Declared after the chunks so its workers stop before chunks are destroyed.
  Thread::ThreadPool m_thread_pool;

  int m_render_distance = 12;
  int m_render_height = 1;

  unsigned int m_chunks_loading;
  unsigned int m_num_chunks;
  unsigned int m_num_chunks_rendered;

  int m_prev_cam_x;
  int m_prev_cam_y;
  int m_prev_cam_z;

  Atmosphere m_atmosphere;
  ModelLoader::Model m_torch_model;
};
