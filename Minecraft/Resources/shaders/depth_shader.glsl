#shader vertex
#version 410 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 uLightSpaceMatrix;

void main() {
  gl_Position = uLightSpaceMatrix * model * vec4(aPos, 1.0f);
}

#shader fragment
#version 410 core
void main() {
  // empty since we dont render
}
