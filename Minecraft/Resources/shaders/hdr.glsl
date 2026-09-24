#shader vertex
#version 410 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
  TexCoord = aTexCoord;
  gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
}

#shader fragment
#version 410 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uHdrTexture;

void main() {
  vec3 hdr_color = texture(uHdrTexture, TexCoord).rgb;
  vec3 mapped_color = hdr_color / (hdr_color + vec3(1.0f));

  FragColor = vec4(mapped_color, 1.0f);
}
