#include "Phys.h"
#include "Blocks.h"
#include "World.h"
#include "constants.h"

std::vector<glm::vec3> broad_phase(World &world, const glm::vec3 &min_pos,
                                   const glm::vec3 &max_pos) {
  // uvec is unsigned int vector, refer to BLOCKS enum in Blocks.h
  std::vector<glm::vec3> blocks;

  // assumming max position is greater than minPos
  blocks.reserve((max_pos.x - min_pos.x) * (max_pos.y - min_pos.y) *
                 (max_pos.z - min_pos.z));

  for (float x = min_pos.x; x < max_pos.x; x += 1.0f) {
    for (float z = min_pos.z; z < max_pos.z; z += 1.0f) {
      for (float y = min_pos.y; y < max_pos.y; y += 1.0f) {
        int chunk_x = std::floor(x / 16);
        int chunk_y = std::floor(y / 16);
        int chunk_z = std::floor(z / 16);

        uvec &chunk_data = world.get_chunk_data(chunk_x, chunk_y, chunk_z);

        if (chunk_data.size() == chunk_size * chunk_size * chunk_size) {
          int x_idx = x - chunk_x * 16;
          int y_idx = y - chunk_y * 16;
          int z_idx = z - chunk_z * 16;

          int vec_index =
              x_idx * chunk_size * chunk_size + z_idx * chunk_size + y_idx;

          // this is safe and avoids safety checks with .at()
          unsigned int state = chunk_data[vec_index];

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

std::vector<ColliderResult> narrow_phase(const std::vector<glm::vec3> &blocks,
                                         glm::vec3 &player_position,
                                         const AABB &player_collider) {
  (void)player_collider; // real overlap test below is now authoritative

  std::vector<ColliderResult> collisions;

  constexpr float half_width = player_width / 2.f;
  constexpr float height = player_height;

  // playerPosition.y is the top of the player box, feet are position.y - height
  const float player_min_x = player_position.x - half_width;
  const float player_max_x = player_position.x + half_width;
  const float player_min_y = player_position.y - height;
  const float player_max_y = player_position.y;
  const float player_min_z = player_position.z - half_width;
  const float player_max_z = player_position.z + half_width;

  for (const glm::vec3 &block : blocks) {
    const float block_min_x = block.x;
    const float block_max_x = block.x + 1.0f;
    const float block_min_y = block.y;
    const float block_max_y = block.y + 1.0f;
    const float block_min_z = block.z;
    const float block_max_z = block.z + 1.0f;

    // real box overlap on all three axes, this replaces the old
    // circle-distance check that didnt match the square player box
    // and caused snagging on block corners and seams
    float overlap_x = std::min(player_max_x, block_max_x) -
                      std::max(player_min_x, block_min_x);
    float overlap_y = std::min(player_max_y, block_max_y) -
                      std::max(player_min_y, block_min_y);
    float overlap_z = std::min(player_max_z, block_max_z) -
                      std::max(player_min_z, block_min_z);

    if (overlap_x <= 0.0f || overlap_y <= 0.0f || overlap_z <= 0.0f)
      continue; // not actually touching on at least one axis

    glm::vec3 contact_point = {
        std::clamp(player_position.x, block_min_x, block_max_x),
        std::clamp(player_position.y, block_min_y, block_max_y),
        std::clamp(player_position.z, block_min_z, block_max_z),
    };

    // y push, pick whichever direction is closer
    float push_up = block_max_y - player_min_y;
    float push_down = player_max_y - block_min_y;
    glm::vec3 y_norm = (push_up < push_down) ? glm::vec3(0.f, 1.f, 0.f)
                                             : glm::vec3(0.f, -1.f, 0.f);
    float y_correction = std::min(push_up, push_down);

    // xz plane push, use whichever of x or z has the smaller overlap
    // this is what keeps a slide along a flat wall smooth

    float push_right = block_max_x - player_min_x;
    float push_left = player_max_x - block_min_x;
    glm::vec3 x_norm = (push_right < push_left) ? glm::vec3(1.f, 0.f, 0.f)
                                                : glm::vec3(-1.f, 0.f, 0.f);
    float x_correction = std::min(push_right, push_left);

    float push_fwd = block_max_z - player_min_z;
    float push_back = player_max_z - block_min_z;
    glm::vec3 z_norm = (push_fwd < push_back) ? glm::vec3(0.f, 0.f, 1.f)
                                              : glm::vec3(0.f, 0.f, -1.f);
    float z_correction = std::min(push_fwd, push_back);

    ColliderResult min_collision{
        glm::vec3(0.0f), contact_point,
        std::min(overlap_x, std::min(overlap_y, overlap_z))};

    if (z_correction <= x_correction && z_correction <= y_correction) {
      min_collision.normal = z_norm;
    } else if (x_correction <= z_correction && x_correction <= y_correction) {
      min_collision.normal = x_norm;
    } else { // min collision is y obv
      min_collision.normal = y_norm;
    }

    collisions.emplace_back(min_collision);
  }

  return collisions;
}
