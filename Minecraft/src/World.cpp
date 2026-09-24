#include "World.h"
#include "FMath.h"
#include "Log.h"
#include "constants.h"
#include "render_defines.h"

#include <algorithm>
#include <vector>

World::World()
    : m_thread_pool(4), m_render_height(1), m_chunks_loading(0),
      m_num_chunks(0), m_num_chunks_rendered(0), m_prev_cam_x(0),
      m_prev_cam_y(20), m_prev_cam_z(0),
      m_torch_model(
          (get_resource_path() / "assets/minecraft_torch.glb").string()) {

  m_atmosphere.init();
  // pre normalize the rgb values for now
  m_atmosphere.add_light({0.0f, 27.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  m_atmosphere.add_light({0.0f, 27.0f, -3.0f}, {0.0f, 0.0f, 1.f});

  // The OpenGL context exists before World is first requested in
  // Minecraft::run

  m_torch_model.init();
}

World::~World() { m_atmosphere.cleanup(); }

void World::update(glm::vec3 cam_pos, Shader *shader, float dt) {

  // we want to floor if we are negative, since int divsion will round up
  int cam_chunk_x = cam_pos.x < 0 ? ftoi_fast(cam_pos.x / chunk_size)
                                  : cam_pos.x / chunk_size;
  int cam_chunk_y = cam_pos.y < 0 ? ftoi_fast(cam_pos.y / chunk_size)
                                  : cam_pos.y / chunk_size;
  int cam_chunk_z = cam_pos.z < 0 ? ftoi_fast(cam_pos.z / chunk_size)
                                  : cam_pos.z / chunk_size;

  // Check if camera moved to new chunk
  if (cam_chunk_x != m_prev_cam_x || cam_chunk_y != m_prev_cam_y ||
      cam_chunk_z != m_prev_cam_z) {

    m_prev_cam_x = cam_chunk_x;
    m_prev_cam_y = cam_chunk_y;
    m_prev_cam_z = cam_chunk_z;

    // chunk we are at
    /*

    we want to prioritize the chunk we are at currently as soon as we can in a
    queue if we dont and we rapidly move to a new area we would be generating
    chunks possibly out of render distance

    */
    m_queue = {};
    if (m_chunks.find({cam_chunk_x, cam_chunk_y, cam_chunk_z}) ==
        m_chunks.end())
      m_queue.push({cam_chunk_x, cam_chunk_y, cam_chunk_z});

    auto enqueue_symmetric = [&](int x, int y, int z) -> void {
      m_queue.push({cam_chunk_x + x, y, cam_chunk_z + z});

      // to avoid duplicates
      if (y != 0)
        m_queue.push({cam_chunk_x + x, -y, cam_chunk_z + z});
    };

    // for each render distance 'step', iterate over the shells of a cube in xyz
    // plane
    // ... where max(|x|, |z|) == step, and enqueue symmetric chunk positions
    // across Y this replaces manual handling of cardinal directions, corners,
    // and edges I had previously

    for (int step = 0; step < m_render_distance; ++step) {
      for (int x = -step; x <= step; ++x) {
        for (int z = -step; z <= step; ++z) {
          // skiping inner cube faces, we only want the shell
          if (std::max(std::abs(x), std::abs(z)) == step) {
            for (int y = 0; y <= m_render_height; ++y) {
              enqueue_symmetric(x, y, z);
            }
          }
        }
      }
    }
  } else if (m_chunks_loading == 0 && !m_queue.empty()) {
    // proccess next item since queue no empty
    glm::vec3 next = m_queue.front();
    m_queue.pop();

    cTuple chunk_tuple{next.x, next.y, next.z};

    if (m_chunks.find(chunk_tuple) == m_chunks.end()) {
      m_chunks.try_emplace(chunk_tuple, next, m_thread_pool, *this);
    }
  }

  m_chunks_loading = 0;
  m_num_chunks = 0;
  m_num_chunks_rendered = 0;
  for (auto it = m_chunks.begin(); it != m_chunks.end();) {
    m_num_chunks++;

    bool chunk_ready = it->second.get_ready_status();

    if (!chunk_ready)
      m_chunks_loading++;

    glm::vec3 &c_pos = it->second.get_pos();

    int chunk_x = c_pos.x;
    int chunk_y = c_pos.y;
    int chunk_z = c_pos.z;

    if (chunk_ready && (abs(chunk_x - cam_chunk_x) > m_render_distance ||
                        abs(chunk_y - cam_chunk_y) > m_render_distance ||
                        abs(chunk_z - cam_chunk_z) > m_render_distance)) {
      it->second.set_render(false);
      m_num_chunks_rendered--;
      ++it;
    } else {
      it->second.on_update();
      it->second.set_render(true);
      m_num_chunks_rendered++;
      ++it;
    }
  }

  // update sun pos here as well but sun pos is static for now
  m_atmosphere.update(shader, dt);
}

void World::render_shadow_pass() {
  Shader *depth_shader = Shader::get_shader("depth_shader");
  depth_shader->bind();
  m_atmosphere.begin_shadow_pass();
  for (auto it = m_chunks.begin(); it != m_chunks.end(); it++) {
    it->second.try_render(depth_shader);
  }

  m_atmosphere.end_shadow_pass();
  depth_shader->unbind();
}

void World::render_scene(Shader *terrain_shader, Shader *model_shader) {
  // set sun pos
  terrain_shader->bind();
  terrain_shader->set_uniform_vec3f("uSunDir",
                                    m_atmosphere.get_sun_direction());
  terrain_shader->set_uniform_mat4f("uLightSpaceMatrix",
                                    m_atmosphere.get_light_space_matrix());
  m_atmosphere.bind_shadow_map();

  for (auto it = m_chunks.begin(); it != m_chunks.end(); it++) {
    it->second.try_render(terrain_shader);
  }

  model_shader->bind();
  for (const glm::vec3 &light_pos : m_atmosphere.get_point_lights().positions) {
    // this should be done in an update and render can stay here
    // todo: move point light update to atmosphere update
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, light_pos);
    model = glm::scale(model, glm::vec3(0.1f));
    model_shader->set_uniform_mat4f("uModel", model);
    m_torch_model.render(model_shader);
  }
}

std::vector<unsigned int> &World::get_chunk_data(int chunk_x, int chunk_y,
                                                 int chunk_z) {
  cTuple chunk_tuple{chunk_x, chunk_y, chunk_z};
  if (m_chunks.find(chunk_tuple) == m_chunks.end()) {
    static std::vector<unsigned int> empty_vector;
    return empty_vector;
  } else {
    return m_chunks.find(chunk_tuple)->second.get_data();
  }
}

void World::mark_neighbors(int lx, int ly, int lz, int cx, int cy, int cz) {
  const int max = chunk_size - 1;

  auto mark_chunk = [&](std::vector<glm::ivec3> &dvecs) -> void {
    for (glm::ivec3 &dvec : dvecs) {
      Chunk *neighbor = get_chunk(cx + dvec.x, cy + dvec.y, cz + dvec.z);

      if (neighbor) {
        neighbor->set_dirty(true);
        neighbor->on_update(); // update duplicated face in the neighbor
      }
    }
  };

  // takes in local indices for lx, ly, lz

  auto get_edge_directions = [](int x, int y,
                                int z) -> std::vector<glm::ivec3> {
    std::vector<glm::ivec3> dirs;

    if (x == 0)
      dirs.push_back({-1, 0, 0}); // -X
    else if (x == chunk_size - 1)
      dirs.push_back({1, 0, 0}); // +X

    if (y == 0)
      dirs.push_back({0, -1, 0}); // -Y
    else if (y == chunk_size - 1)
      dirs.push_back({0, 1, 0}); // +Y

    if (z == 0)
      dirs.push_back({0, 0, -1}); // -Z
    else if (z == chunk_size - 1)
      dirs.push_back({0, 0, 1}); // +Z

    return dirs;
  };

  // Check if the block is on the edge of the chunk
  std::vector<glm::ivec3> dirs = get_edge_directions(lx, ly, lz);
  mark_chunk(dirs);
}
