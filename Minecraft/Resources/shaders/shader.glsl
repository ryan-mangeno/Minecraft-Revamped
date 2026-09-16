#shader vertex
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 TotalColoring;
out vec4 FragPosLightSpace;

uniform vec3 uSunDir;
uniform vec3 uCamPos;
uniform float uAmbientStrength;
uniform float uSpecularStrength;
uniform float uShininess;
uniform sampler2D tex;

const int MAX_POINT_LIGHTS = 16;
uniform int uPointLightCount;
uniform vec3 uPointLightPositions[MAX_POINT_LIGHTS];
uniform vec3 uPointLightColors[MAX_POINT_LIGHTS];

uniform mat4 uLightSpaceMatrix;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 WorldPos = vec3(model * vec4(aPos, 1.0));
    FragPosLightSpace = uLightSpaceMatrix * vec4(WorldPos, 1.0f);

    gl_Position = projection * view * vec4(WorldPos, 1.0);

    TexCoord = aTexCoord / vec2(textureSize(tex, 0));

    vec3 viewDir = normalize(uCamPos - WorldPos);
    vec3 reflectDir = reflect(uSunDir, aNormal);

    float specular = uSpecularStrength * pow(max(0.0, dot(reflectDir, viewDir)), uShininess);
    float diffuse = max(0.0, dot(aNormal, -uSunDir));

    float brightness = specular + diffuse + uAmbientStrength;
    // start with sun coloring, white for now
    TotalColoring = vec3(1.0f) * brightness;
    for (int i = 0; i < uPointLightCount; ++i) {
        vec3 toLight = uPointLightPositions[i] - WorldPos;
        float distanceToLight = length(toLight);
        vec3 pointLightDir = normalize(toLight);
        float pointDiffuse = max(0.0, dot(aNormal, pointLightDir));
        float attenuation = 1.0 / (1.0 + distanceToLight * distanceToLight);
        float intensity = 0.3;
        float point_brightness = pointDiffuse * attenuation * intensity;
        TotalColoring += point_brightness * uPointLightColors[i];
    }
}

#shader fragment
#version 410 core

in vec2 TexCoord;
in vec3 TotalColoring;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform sampler2D tex;
uniform sampler2D uShadowMap;

void main()
{
    vec3 frag_pos = FragPosLightSpace.xyz / FragPosLightSpace.w;
    frag_pos = frag_pos * 0.5 + 0.5; // remap from [-1,1] to [0,1]
    float stored_depth = texture(uShadowMap, frag_pos.xy).r;
    float cur_depth = frag_pos.z;
    float brightness = 0.5;
    float bias = 0.05;
    // if its visable to sun
    if (cur_depth - bias <= stored_depth) {
        brightness = 1.0f;
    }
	FragColor = texture(tex, TexCoord) * (vec4(TotalColoring, 1.0f) * brightness);
}
