#include "Ray.h"
#include "Blocks.h"
#include "World.h"
#include "constants.h"

#include <queue>
#include <unordered_set>

Ray::Ray(const glm::vec3 &start) : m_start_position(start) {}

static bool ray_aabb_intersect(const glm::vec3 &ray_origin,
                             const glm::vec3 &ray_dir, const glm::vec3 &aabb_min,
                             const glm::vec3 &aabb_max, float &tmin,
                             float &tmax) {
  glm::vec3 inv_dir = 1.0f / ray_dir;
  glm::vec3 t0 = (aabb_min - ray_origin) * inv_dir;
  glm::vec3 t1 = (aabb_max - ray_origin) * inv_dir;

  tmin = std::max(std::max(std::min(t0.x, t1.x), std::min(t0.y, t1.y)),
                  std::min(t0.z, t1.z));
  tmax = std::min(std::min(std::max(t0.x, t1.x), std::max(t0.y, t1.y)),
                  std::max(t0.z, t1.z));

  return tmax >= tmin && tmax >= 0.0f;
}

// floor-division for chunk coords - plain '/' truncates toward zero and
// puts negative-space blocks in the wrong chunk (e.g. -1 / 16 == 0, but
// block -1 belongs to chunk -1). Matches the std::floor(x/16) broad_phase
// already uses in Phys.cpp.
static inline int floor_div(int a, int b) {
  return (a >= 0) ? (a / b) : -(((-a) + b - 1) / b);
}

// for now ray casting will just return wether it hits a block or not

struct BlockHit {
  glm::ivec3 pos;
  float t; // distance along the ray to this block
  bool operator>(const BlockHit &other) const { return t > other.t; }
};

bool Ray::cast(World &world, const glm::vec3 &direction, float max_dist) {
  glm::vec3 dir = glm::normalize(direction);
  glm::vec3 start = m_start_position;
  auto hash_vec = [](const glm::ivec3 &v) {
    return std::hash<int>()(v.x) ^ std::hash<int>()(v.y << 1) ^
           std::hash<int>()(v.z << 2);
  };

  std::priority_queue<BlockHit, std::vector<BlockHit>, std::greater<BlockHit>>
      q;
  std::unordered_set<glm::ivec3, decltype(hash_vec)> visited(0, hash_vec);

  glm::ivec3 current = glm::floor(start);
  q.push({current, 0.0f});
  visited.insert(current);

  while (!q.empty()) {
    BlockHit block = q.top();
    q.pop();
    if (block.t > max_dist)
      break;

    // Check the block
    int cx = floor_div(block.pos.x, chunk_size);
    int cy = floor_div(block.pos.y, chunk_size);
    int cz = floor_div(block.pos.z, chunk_size);

    uvec &chunk_data = world.get_chunk_data(cx, cy, cz);
    Chunk *chunk = world.get_chunk(cx, cy, cz);

    // skip unloaded/ungenerated chunks instead of throwing (.at()) or
    // null-dereferencing chunk->set_block below - still fall through to
    // neighbor expansion so the ray keeps traveling past this gap
    bool chunk_ready = chunk != nullptr &&
                       chunk_data.size() == chunk_size * chunk_size * chunk_size;

    if (chunk_ready) {
      int lx = block.pos.x % chunk_size;
      int ly = block.pos.y % chunk_size;
      int lz = block.pos.z % chunk_size;

      if (lx < 0)
        lx += chunk_size;
      if (ly < 0)
        ly += chunk_size;
      if (lz < 0)
        lz += chunk_size;

      int index = lx * chunk_size * chunk_size + lz * chunk_size + ly;

      if (chunk_data[index] != Blocks::AIR) {
        chunk->set_block(index, Blocks::AIR); // remove the block
        chunk->set_dirty(true);               // mark the chunk as dirty
        world.mark_neighbors(lx, ly, lz, cx, cy, cz);

        std::cout << "hit" << block.pos.x << " " << block.pos.y << " "
                  << block.pos.z << std::endl;
        return true;
      }
    }

    // Add neighbors in axis order (1-axis first, then 2-axis moves, etc.)
    for (int dx = -1; dx <= 1; ++dx)
      for (int dy = -1; dy <= 1; ++dy)
        for (int dz = -1; dz <= 1; ++dz) {
          glm::ivec3 neighbor = block.pos + glm::ivec3(dx, dy, dz);
          if (visited.count(neighbor))
            continue;

          // compute t (entry distance into this block)
          glm::vec3 min_bound = glm::vec3(neighbor);
          glm::vec3 max_bound = min_bound + glm::vec3(1.0f);
          float t_near, t_far;
          if (ray_aabb_intersect(start, dir, min_bound, max_bound, t_near, t_far)) {
            q.push({neighbor, t_near});
            visited.insert(neighbor);
          }
        }
  }

  return false;
}
