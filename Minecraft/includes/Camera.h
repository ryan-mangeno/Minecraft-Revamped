#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "constants.h"
#include "math_util.h"

class Camera {
public:
  glm::mat4 calc_view_matrix() const;

  inline const glm::vec3 &get_pos() const { return m_position; }
  inline const glm::mat4 &get_proj_mat() const { return m_projection_mat; }
  inline const float get_zoom() const { return m_zoom; }
  inline const glm::vec3 &get_orientation() const { return m_orientation; }

  void dispatch_keyboard_event(Direction direction, float delta_time);
  void dispatch_mouse_move_event(float xrot, float yrot);
  void dispatch_mouse_scroll_event(float scroll);

  void on_update(float delta_time);

  // singleton for camera, only one camera for minecraft
  static Camera &get_camera() {
    static Camera cam(glm::vec3(0.0f, 25.0f, 0.0f), screen_width, screen_height,
                      45.f, 0.1f, 100.f);
    return cam;
  }

private:
  Camera(glm::vec3 position, int width, int height, float fov, float near_plane,
         float far_plane);

  // projection stuff
  glm::mat4 m_mvp;
  glm::mat4 m_projection_mat;

  // not adding model mat, will define in maybe super class with multiplayer
  //  the model mats will define the offset of the player models for each player
  //  in game

  // camera attribs
  glm::vec3 m_position;
  glm::vec3 m_position_update;
  glm::vec3 m_prev_position;

  glm::vec3 m_orientation;
  glm::vec3 m_right;
  glm::vec3 m_up;

  // rotation angles
  float m_yaw;
  float m_pitch;

  unsigned int m_width;
  unsigned int m_height;

  float m_speed;
  float m_velocity;
  float m_sensitivity;
  float m_zoom;

  void update_camera_vectors();
};
