#shader vertex
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out float Brightness;

uniform vec3 uSunPos;
uniform vec3 uCamPos;
uniform float uAmbient;
uniform float uSpecular;
uniform sampler2D tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 worldPos = vec3(model * vec4(aPos, 1.0));

    gl_Position = projection * view * vec4(worldPos, 1.0);

    TexCoord = aTexCoord / vec2(textureSize(tex, 0));

    vec3 lightDir = normalize(uSunPos - worldPos);
    vec3 viewDir = normalize(uCamPos - worldPos);
    vec3 reflectDir = reflect(-lightDir, aNormal);

    float specular = pow(max(0.0, dot(reflectDir, viewDir)), uSpecular);
    float diffuse = max(0.0, dot(aNormal, lightDir));

    Brightness = specular + diffuse + uAmbient;
}

#shader fragment
#version 410 core

in vec2 TexCoord;
in float Brightness;

out vec4 FragColor;

uniform sampler2D tex;

void main()
{
	FragColor = texture(tex, TexCoord) * Brightness;
}
