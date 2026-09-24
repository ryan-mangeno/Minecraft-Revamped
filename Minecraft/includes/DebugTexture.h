#ifndef DEBUG_TEXTURE_H
#define DEBUG_TEXTURE_H

#include <cstdint>

struct DebugTexture {
  const char *name;
  uint32_t texture_id;
  int width;
  int height;
  bool flip_vertical{true};
};

#endif
