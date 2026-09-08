#include "Camera.h"
#include "Log.h"
#include "Phys.h"

// amplifying gravity * 2
const float gravity = -9.81f * 2;
const float terminal_velocity = -50.0f;

// general up direction for projection stuff
constexpr glm::vec3 g_up(0.f, 1.f, 0.f);

Camera::Camera(glm::vec3 position, int width, int height, float fov,
               float near_plane, float far_plane)
    : m_width(width), m_height(height), m_position(position),
      m_position_update(position), m_prev_position(position),
      m_mvp(glm::mat4(1.0f)), m_orientation(0.0f, 0.0f, -1.0f),
      m_up(glm::vec3(0.0f, 1.0f, 0.0f)), m_right(glm::vec3(1.0f, 0.0f, 0.0f)),
      m_yaw(-90.f), m_pitch(0.f), m_zoom(75.0f), m_speed(5.0f),
      m_velocity(5.0f), m_sensitivity(0.5f) {
  // can do fov or zoom for this
  m_projection_mat =
      glm::perspective(glm::radians(m_zoom), float(m_width) / float(m_height),
                       near_plane, far_plane);
  update_camera_vectors();
}

glm::mat4 Camera::calc_view_matrix() const {
  return glm::lookAt(m_position, m_position + m_orientation, g_up);
}
void Camera::dispatch_keyboard_event(Direction dir, float delta_time) {

  float velocity = m_speed * delta_time;

  glm::vec3 backward = glm::cross(m_right, g_up);

  switch (dir) {
  case NORTH:
    m_position_update += -backward * velocity;
    break;
  case SOUTH:
    m_position_update += backward * velocity;
    break;
  case WEST:
    m_position_update += -m_right * velocity;
    break;
  case EAST:
    m_position_update += m_right * velocity;
    break;
  case UP:
    m_position_update += g_up * velocity;
    m_velocity = 5.0f;
    break;
  case DOWN:
    m_position_update += -g_up * velocity;
    break;
  case NONE:
    break;

  default:
    break;
  }
}

// for gravity sim
void Camera::on_update(float delta_time) {
  // resolve xz first at current height, then y after gravity
  // a pure side hit can't leak into a vertical push this way

  // narrowphase now does a real box vs box overlap test itself and
  // only returns actual collisions, so no gate check is needed here
  auto query_hits = [this]() -> std::vector<ColliderResult> {
    std::vector<glm::vec3> blocks =
        broad_phase(glm::floor(m_position_update - 2.0f),
                   glm::ceil(m_position_update + 4.0f));
    glm::vec3 min_box_pos{m_position_update.x - player_width / 2.f,
                        m_position_update.y - player_height,
                        m_position_update.z - player_width / 2.0f};
    AABB box(min_box_pos, player_width, player_height);
    return narrow_phase(blocks, m_position_update, box);
  };

  // horizontal pass
  {
    auto hit_blocks = query_hits();
    glm::vec3 total_correction_xz(0.0f);
    for (const auto &it : hit_blocks) {
      // keep the biggest correction instead of overwriting
      if (it.overlap_xz != 0.0f) {
        glm::vec3 c = it.normal * it.overlap_xz;
        if (glm::length(c) > glm::length(total_correction_xz))
          total_correction_xz = c;
      }
    }
    m_position_update += total_correction_xz;
  }

  // vertical pass
  m_velocity += gravity * delta_time;
  if (m_velocity < terminal_velocity)
    m_velocity = terminal_velocity;
  m_position_update.y += m_velocity * delta_time;

  {
    auto hit_blocks = query_hits();

    glm::vec3 total_correction_y(0.0f);
    for (const auto &it : hit_blocks) {
      if (it.overlap_y != 0.0f) {
        glm::vec3 c = it.normal * it.overlap_y;
        if (glm::length(c) > glm::length(total_correction_y))
          total_correction_y = c;
        m_velocity = 0.0f;
      }
    }

    // this is a distance, not a velocity, dont scale by deltaTime
    m_position_update += total_correction_y;
  }

  m_prev_position = m_position;
  m_position = m_position_update;
}

// @param xrot -  x offset/rot from different between cur mouse x and prev mouse
// x
// @param yrot - y offset/rot from different between cur mouse y and prev mouse
// y
void Camera::dispatch_mouse_move_event(float xrot, float yrot) {

  // amplify rot based on sens
  xrot *= m_sensitivity;
  yrot *= m_sensitivity;

  // yaw is rot around y axis, so its x offset
  m_yaw += xrot;
  m_pitch += yrot;

  if (!(m_pitch >= -85.0f && m_pitch <= 85.0f)) {
    m_pitch -= yrot;
  }

  update_camera_vectors();
}

void Camera::dispatch_mouse_scroll_event(float scroll) {
  m_speed += scroll;

  // if we go too low
  m_speed = (m_speed < 0) ? 0 : m_speed;
}

// calculates the front vector from the camera's updated euler angles
void Camera::update_camera_vectors() {
  // Yaw is circular: crossing either boundary should continue from the
  // equivalent angle on the other side instead of stopping rotation.
  if (m_yaw > 180.0f)
    m_yaw -= 360.0f;
  else if (m_yaw < -180.0f)
    m_yaw += 360.0f;

  // cos m_pitch dampens x rot when looking very high or low
  m_orientation.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

  // simply sin for y component
  m_orientation.y = sin(glm::radians(m_pitch));

  m_orientation.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

  m_orientation = glm::normalize(m_orientation);

  // ori changed so we need to recalc up and right
  m_right = glm::normalize(glm::cross(m_orientation, g_up));
  m_up = glm::normalize(glm::cross(m_right, m_orientation));
}
