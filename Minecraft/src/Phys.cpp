#include "Phys.h"
#include "constants.h"
#include "math_util.h"

std::vector<glm::vec3> BroadPhase(const glm::vec3 &minPos,
                                  const glm::vec3 &maxPos) {
  World &world = World::GetWorld();

  // uvec is unsigned int vector, refer to BLOCKS enum in Blocks.h
  std::vector<glm::vec3> blocks;

  // assumming max position is greater than minPos
  blocks.reserve((maxPos.x - minPos.x) * (maxPos.y - minPos.y) *
                 (maxPos.z - minPos.z));

  for (float x = minPos.x; x < maxPos.x; x += 1.0f) {
    for (float z = minPos.z; z < maxPos.z; z += 1.0f) {
      for (float y = minPos.y; y < maxPos.y; y += 1.0f) {
        int chunkX = std::floor(x / 16);
        int chunkY = std::floor(y / 16);
        int chunkZ = std::floor(z / 16);

        uvec &chunkData = world.GetChunkData(chunkX, chunkY, chunkZ);

        if (chunkData.size() == CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE) {
          int xIdx = x - chunkX * 16;
          int yIdx = y - chunkY * 16;
          int zIdx = z - chunkZ * 16;

          int vecIndex =
              xIdx * CHUNK_SIZE * CHUNK_SIZE + zIdx * CHUNK_SIZE + yIdx;

          // this is safe and avoids safety checks with .at()
          unsigned int state = chunkData[vecIndex];

          if (state != Blocks::AIR) {
            blocks.emplace_back(x, y, z);
          }
        }
      }
    }
  }
  // std::cout << blocks.size() << '\n';

  return blocks;
}

std::vector<ColliderResult> NarrowPhase(const std::vector<glm::vec3> &blocks,
                                        glm::vec3 &playerPosition,
                                        const AABB &playerCollider) {
  std::vector<ColliderResult> collisions; // todo: max collisions 4?, make
                                          // static array and memset every time?

  constexpr float radius = PLAYER_WIDTH / 2.f;
  constexpr float height = PLAYER_HEIGHT;
  constexpr float dampingFactor = 0.1f;
  constexpr float ampFactor = 20.0f;

  for (const glm::vec3 &block : blocks) {
    // block coords start at bottom left
    glm::vec3 closestPoint = {
        std::max(block.x, std::min(playerPosition.x, block.x + 1.0f)),
        std::max(block.y, std::min(playerPosition.y, block.y + 1.0f)),
        std::max(block.z, std::min(playerPosition.z, block.z + 1.0f)),
    };

    bool is_above = false;

    float dx = closestPoint.x - playerPosition.x;

    float dy = closestPoint.y - playerPosition.y;
    if (playerPosition.y > block.y + 1.0f) {
      dy -= PLAYER_HEIGHT / 2.f;
      is_above = true;
    }

    float dz = closestPoint.z - playerPosition.z;
    float r_sq = dx * dx + dz * dz;

    bool collision =
        playerCollider.collides(closestPoint.x, closestPoint.y, closestPoint.z);

    if (collision) {

      float overlapY = height / 2.f - dy;

      if (is_above)
        overlapY *=
            dampingFactor; // applying damping to prevent continuous bouncing
      else
        overlapY *= ampFactor;

      float overlapXZ = std::sqrt(r_sq) - radius;
      glm::vec3 yNorm{0.f, (dy < -1.0f) - (dy > -1.0f), 0.f};
      glm::vec3 xzNorm(0.0f);
      if (dx != 0.0f && dz != 0.0f)
        xzNorm = glm::normalize(glm::vec3{-dx, 0.0f, -dz});

      // temporarily combining xz and y collisions for testing.
      collisions.emplace_back(yNorm, closestPoint, overlapY, 0.0f);
      collisions.emplace_back(xzNorm, closestPoint, 0.0f, overlapXZ);
    }
  }

  return collisions;
}
