#shader vertex
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);

    WorldPos = worldPosition.xyz;
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    TexCoord = aTexCoord;

    gl_Position = projection * view * worldPosition;
}

#shader fragment
#version 410 core

in vec2 TexCoord;
in vec3 WorldPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main()
{
    vec4 baseColor = texture(texture_diffuse1, TexCoord);

    // TODO: Apply your lighting equation here. WorldPos and Normal are already
    // available, so this shader is ready to receive the same light uniforms as
    // the terrain shader.
    FragColor = baseColor;
}
