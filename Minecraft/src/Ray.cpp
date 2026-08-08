#include "Ray.h"
#include "Blocks.h"
#include "Ray.h"
#include "World.h"
#include "constants.h"

#include <queue>
#include <unordered_set>

Ray::Ray(const glm::vec3 &start) : m_StartPosition(start) {}

static bool RayAABBIntersect(const glm::vec3 &rayOrigin,
                             const glm::vec3 &rayDir, const glm::vec3 &aabbMin,
                             const glm::vec3 &aabbMax, float &tmin,
                             float &tmax) {
  glm::vec3 invDir = 1.0f / rayDir;
  glm::vec3 t0 = (aabbMin - rayOrigin) * invDir;
  glm::vec3 t1 = (aabbMax - rayOrigin) * invDir;

  glm::vec3 tminVec = glm::min(t0, t1);
  glm::vec3 tmaxVec = glm::max(t0, t1);

  tmin = std::max(std::max(std::min(t0.x, t1.x), std::min(t0.y, t1.y)),
                  std::min(t0.z, t1.z));
  tmax = std::min(std::min(std::max(t0.x, t1.x), std::max(t0.y, t1.y)),
                  std::max(t0.z, t1.z));

  return tmax >= tmin && tmax >= 0.0f;
}

// for now ray casting will just return wether it hits a block or not

struct BlockHit {
  glm::ivec3 pos;
  float t; // distance along the ray to this block
  bool operator>(const BlockHit &other) const { return t > other.t; }
};

bool Ray::Cast(const glm::vec3 &direction, float maxDist) {
  glm::vec3 dir = glm::normalize(direction);
  glm::vec3 start = m_StartPosition;
  World &world = World::GetWorld();

  auto hashVec = [](const glm::ivec3 &v) {
    return std::hash<int>()(v.x) ^ std::hash<int>()(v.y << 1) ^
           std::hash<int>()(v.z << 2);
  };

  std::priority_queue<BlockHit, std::vector<BlockHit>, std::greater<BlockHit>>
      q;
  std::unordered_set<glm::ivec3, decltype(hashVec)> visited(0, hashVec);

  glm::ivec3 current = glm::floor(start);
  q.push({current, 0.0f});
  visited.insert(current);

  while (!q.empty()) {
    BlockHit block = q.top();
    q.pop();
    if (block.t > maxDist)
      break;

    // Check the block
    int cx = block.pos.x / CHUNK_SIZE;
    int cy = block.pos.y / CHUNK_SIZE;
    int cz = block.pos.z / CHUNK_SIZE;

    uvec &chunkData = world.GetChunkData(cx, cy, cz);
    Chunk *chunk = world.GetChunk(cx, cy, cz);

    int lx = block.pos.x % CHUNK_SIZE;
    int ly = block.pos.y % CHUNK_SIZE;
    int lz = block.pos.z % CHUNK_SIZE;

    if (lx < 0)
      lx += CHUNK_SIZE;
    if (ly < 0)
      ly += CHUNK_SIZE;
    if (lz < 0)
      lz += CHUNK_SIZE;

    int index = lx * CHUNK_SIZE * CHUNK_SIZE + lz * CHUNK_SIZE + ly;

    if (chunkData.at(index) != Blocks::AIR) {
      chunk->SetBlock(index, Blocks::AIR); // remove the block
      chunk->SetDirty(true);               // mark the chunk as dirty
      world.MarkNeighbors(lx, ly, lz, cx, cy, cz);

      std::cout << "hit" << block.pos.x << " " << block.pos.y << " "
                << block.pos.z << std::endl;
      return true;
    }

    // Add neighbors in axis order (1-axis first, then 2-axis moves, etc.)
    for (int dx = -1; dx <= 1; ++dx)
      for (int dy = -1; dy <= 1; ++dy)
        for (int dz = -1; dz <= 1; ++dz) {
          glm::ivec3 neighbor = block.pos + glm::ivec3(dx, dy, dz);
          if (visited.count(neighbor))
            continue;

          // compute t (entry distance into this block)
          glm::vec3 minBound = glm::vec3(neighbor);
          glm::vec3 maxBound = minBound + glm::vec3(1.0f);
          float tNear, tFar;
          if (RayAABBIntersect(start, dir, minBound, maxBound, tNear, tFar)) {
            q.push({neighbor, tNear});
            visited.insert(neighbor);
          }
        }
  }

  return false;
}
