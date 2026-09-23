#ifndef MINECRAFT_H
#define MINECRAFT_H

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Minecraft {

private:
  bool init_gl();

public:
  void init();
  void run();

  Minecraft() {}
  ~Minecraft();
};

#endif
