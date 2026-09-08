#include "Phys.h"
#include "constants.h"
#include "math_util.h"

std::vector<glm::vec3> broad_phase(const glm::vec3 &min_pos,
                                  const glm::vec3 &max_pos) {
  World &world = World::get_world();

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
    float overlap_x = std::min(player_max_x, block_max_x) - std::max(player_min_x, block_min_x);
    float overlap_y = std::min(player_max_y, block_max_y) - std::max(player_min_y, block_min_y);
    float overlap_z = std::min(player_max_z, block_max_z) - std::max(player_min_z, block_min_z);

    if (overlap_x <= 0.0f || overlap_y <= 0.0f || overlap_z <= 0.0f)
      continue; // not actually touching on at least one axis

    glm::vec3 contact_point = {
        std::clamp(player_position.x, block_min_x, block_max_x),
        std::clamp(player_position.y, block_min_y, block_max_y),
        std::clamp(player_position.z, block_min_z, block_max_z),
    };

    // y push, pick whichever direction is closer
    {
      float push_up   = block_max_y - player_min_y;
      float push_down = player_max_y - block_min_y;
      glm::vec3 y_norm = (push_up < push_down) ? glm::vec3(0.f, 1.f, 0.f)
                                             : glm::vec3(0.f, -1.f, 0.f);
      float correction = std::min(push_up, push_down);
      collisions.emplace_back(y_norm, contact_point, correction, 0.0f);
    }

    // xz push, use whichever of x or z has the smaller overlap
    // this is what keeps a slide along a flat wall smooth
    {
      glm::vec3 xz_norm(0.0f);
      float correction = 0.0f;

      if (overlap_x < overlap_z) {
        float push_right = block_max_x - player_min_x;
        float push_left  = player_max_x - block_min_x;
        xz_norm = (push_right < push_left) ? glm::vec3(1.f, 0.f, 0.f) : glm::vec3(-1.f, 0.f, 0.f);
        correction = std::min(push_right, push_left);
      } else {
        float push_fwd  = block_max_z - player_min_z;
        float push_back = player_max_z - block_min_z;
        xz_norm = (push_fwd < push_back) ? glm::vec3(0.f, 0.f, 1.f) : glm::vec3(0.f, 0.f, -1.f);
        correction = std::min(push_fwd, push_back);
      }

      collisions.emplace_back(xz_norm, contact_point, 0.0f, correction);
    }
  }

  return collisions;
}
