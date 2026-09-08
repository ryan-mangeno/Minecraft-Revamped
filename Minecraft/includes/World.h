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
  void render(Shader *terrain_shader, Shader *model_shader);

  inline void set_dirty_chunk(int chunk_x, int chunk_y, int chunk_z) {
    cTuple chunk_tuple{chunk_x, chunk_y, chunk_z};
    if (m_chunks.find(chunk_tuple) != m_chunks.end()) {
      m_chunks[chunk_tuple].set_dirty(true);
    }
  }

  inline Chunk *get_chunk(int chunk_x, int chunk_y, int chunk_z) {
    cTuple chunk_tuple{chunk_x, chunk_y, chunk_z};
    if (m_chunks.find(chunk_tuple) != m_chunks.end()) {
      return &m_chunks[chunk_tuple];
    } else {
      return nullptr;
    }
  }

  static World &get_world() {
    static World w;
    return w;
  }

  void mark_neighbors(int lx, int ly, int lz, int cx, int cy, int cz);

private:
  // for chunk generation
  Thread::ThreadPool m_thread_pool;

  std::unordered_map<cTuple, Chunk> m_chunks;
  std::queue<glm::vec3> m_queue;

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
