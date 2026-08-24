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
  (void)playerCollider; // real overlap test below is now authoritative

  std::vector<ColliderResult> collisions;

  constexpr float halfWidth = PLAYER_WIDTH / 2.f;
  constexpr float height = PLAYER_HEIGHT;

  // playerPosition.y is the top of the player box, feet are position.y - height
  const float playerMinX = playerPosition.x - halfWidth;
  const float playerMaxX = playerPosition.x + halfWidth;
  const float playerMinY = playerPosition.y - height;
  const float playerMaxY = playerPosition.y;
  const float playerMinZ = playerPosition.z - halfWidth;
  const float playerMaxZ = playerPosition.z + halfWidth;

  for (const glm::vec3 &block : blocks) {
    const float blockMinX = block.x;
    const float blockMaxX = block.x + 1.0f;
    const float blockMinY = block.y;
    const float blockMaxY = block.y + 1.0f;
    const float blockMinZ = block.z;
    const float blockMaxZ = block.z + 1.0f;

    // real box overlap on all three axes, this replaces the old
    // circle-distance check that didnt match the square player box
    // and caused snagging on block corners and seams
    float overlapX = std::min(playerMaxX, blockMaxX) - std::max(playerMinX, blockMinX);
    float overlapY = std::min(playerMaxY, blockMaxY) - std::max(playerMinY, blockMinY);
    float overlapZ = std::min(playerMaxZ, blockMaxZ) - std::max(playerMinZ, blockMinZ);

    if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f)
      continue; // not actually touching on at least one axis

    glm::vec3 contactPoint = {
        std::clamp(playerPosition.x, blockMinX, blockMaxX),
        std::clamp(playerPosition.y, blockMinY, blockMaxY),
        std::clamp(playerPosition.z, blockMinZ, blockMaxZ),
    };

    // y push, pick whichever direction is closer
    {
      float pushUp   = blockMaxY - playerMinY;
      float pushDown = playerMaxY - blockMinY;
      glm::vec3 yNorm = (pushUp < pushDown) ? glm::vec3(0.f, 1.f, 0.f)
                                             : glm::vec3(0.f, -1.f, 0.f);
      float correction = std::min(pushUp, pushDown);
      collisions.emplace_back(yNorm, contactPoint, correction, 0.0f);
    }

    // xz push, use whichever of x or z has the smaller overlap
    // this is what keeps a slide along a flat wall smooth
    {
      glm::vec3 xzNorm(0.0f);
      float correction = 0.0f;

      if (overlapX < overlapZ) {
        float pushRight = blockMaxX - playerMinX;
        float pushLeft  = playerMaxX - blockMinX;
        xzNorm = (pushRight < pushLeft) ? glm::vec3(1.f, 0.f, 0.f) : glm::vec3(-1.f, 0.f, 0.f);
        correction = std::min(pushRight, pushLeft);
      } else {
        float pushFwd  = blockMaxZ - playerMinZ;
        float pushBack = playerMaxZ - blockMinZ;
        xzNorm = (pushFwd < pushBack) ? glm::vec3(0.f, 0.f, 1.f) : glm::vec3(0.f, 0.f, -1.f);
        correction = std::min(pushFwd, pushBack);
      }

      collisions.emplace_back(xzNorm, contactPoint, 0.0f, correction);
    }
  }

  return collisions;
}
