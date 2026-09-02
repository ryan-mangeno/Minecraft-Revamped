#shader vertex
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out float Brightness;

uniform vec3 uSunPos;
uniform vec3 uCamPos;
uniform float uAmbientStrength;
uniform float uSpecularStrength;
uniform float uShininess;
uniform sampler2D tex;

const int MAX_POINT_LIGHTS = 16;
uniform int uPointLightCount;
uniform vec3 uPointLightPositions[MAX_POINT_LIGHTS];

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

    float specular = uSpecularStrength * pow(max(0.0, dot(reflectDir, viewDir)), uShininess);
    float diffuse = max(0.0, dot(aNormal, lightDir));

    Brightness = specular + diffuse + uAmbientStrength;

    // todo: add coloring to point lights
    for (int i = 0; i < uPointLightCount; ++i) {
        vec3 toLight = uPointLightPositions[i] - worldPos;
        float distanceToLight = length(toLight);
        vec3 pointLightDir = normalize(toLight);
        float pointDiffuse = max(0.0, dot(aNormal, pointLightDir));
        float attenuation = 1.0 / (1.0 + distanceToLight * distanceToLight);
        Brightness += pointDiffuse * attenuation;
    }
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
