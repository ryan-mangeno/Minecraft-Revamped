#pragma once

#include "constants.h"
#include <iostream>

class AppAttribs {
public:
  AppAttribs(int width, int height)
      : m_frame_buff_width(width), m_frame_buff_height(height), m_menu_mode(false),
        m_first_mouse(false), m_escape_pressed(false), m_prev_mouse_x(400),
        m_prev_mouse_y(300), m_delta_time(0) {}

  static AppAttribs &get_app_attribs() {
    static AppAttribs attribs(screen_width, screen_height);
    return attribs;
  }

  inline void set_frame_buff_sizes(int width, int height) {
    m_frame_buff_width = width;
    m_frame_buff_height = height;
  }

  inline void set_escaped(bool escaped) { m_escape_pressed = escaped; }

  inline void set_menu_mode(bool menu_mode) { m_menu_mode = menu_mode; }

  inline void set_first_mouse(bool first_mouse) { m_first_mouse = first_mouse; }

  inline void set_prev_mouse_pos(double x, double y) {
    m_prev_mouse_x = x;
    m_prev_mouse_y = y;
  }

  inline double get_prev_mouse_x() { return m_prev_mouse_x; }

  inline double get_prev_mouse_y() { return m_prev_mouse_y; }

  inline void invert_menu_mode_status() { m_menu_mode = !m_menu_mode; }

  inline void set_delta_time(double dt) { m_delta_time = dt; }

  inline double get_delta_time() { return m_delta_time; }

  inline int get_width() { return m_frame_buff_width; }
  inline int get_height() { return m_frame_buff_height; }
  inline bool get_escaped() { return m_escape_pressed; }
  inline bool get_menu_mode() { return m_menu_mode; }
  inline bool get_first_mouse() { return m_first_mouse; }

private:
  double m_delta_time;

  double m_prev_mouse_x;
  double m_prev_mouse_y;

  int m_frame_buff_width, m_frame_buff_height;

  bool m_escape_pressed;
  bool m_menu_mode;
  bool m_first_mouse;
};
