#shader vertex
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;

out vec3 Normal;
out vec2 TexCoord;
out vec3 Tangent;
out vec3 aBiTangent;

out vec4 FragPosLightSpace;
out vec3 WorldPos;

uniform mat4 uLightSpaceMatrix;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    WorldPos = vec3(model * vec4(aPos, 1.0));
    FragPosLightSpace = uLightSpaceMatrix * vec4(WorldPos, 1.0f);
    TexCoord = aTexCoord;
    Normal = aNormal;
    Tangent = aTangent;
    BiTangent = aBiTangent;

    gl_Position = projection * view * vec4(WorldPos, 1.0);

}

#shader fragment
#version 410 core

in vec4 FragPosLightSpace;
in vec3 Normal;
in vec3 WorldPos;
in vec2 TexCoord;
in vec3 Tangent;
in vec3 BiTangent;

uniform vec3 uSunDir;
uniform vec3 uCamPos;

uniform float uAmbientStrength;
uniform float uSpecularStrength;
uniform float uShininess;

uniform sampler2D uAtlas;
uniform sampler2D uAtlasNormal;
uniform sampler2D uShadowMap;

const int MAX_POINT_LIGHTS = 16;
uniform int uPointLightCount;
uniform vec3 uPointLightPositions[MAX_POINT_LIGHTS];
uniform vec3 uPointLightColors[MAX_POINT_LIGHTS];

out vec4 FragColor;

float pcs_sample(float x, float y, float cur_depth, float bias, float len) {
    float bound = floor(len/2.0f);
    float top = y + bound / 2048.0f;
    float bottom = y - bound / 2048.0f;
    float left = x - bound / 2048.0f;
    float right = x + bound / 2048.0f;
    float step = (1.0f / 2048.0f);
    float sun_visibility = 0.0f;

    for (float y_i = top ; y_i >= bottom ; y_i -= step) {
        for (float x_i = left ; x_i <= right; x_i += step) {
            float stored_depth = texture(uShadowMap, vec2(x_i, y_i)).r;
            if (cur_depth - bias <= stored_depth) {
                sun_visibility += 1.0f;
            }
        }
    }
    return sun_visibility / (len*len);
}

void main()
{
    vec2 tex_coord = TexCoord / vec2(textureSize(uAtlas, 0));

    // [0,1] -> [-1,1]
    vec3 normal = normalize(((texture(uAtlasNormal, tex_coord).rgb * 2.0f) - 1.0f) + Normal);

    vec3 viewDir = normalize(uCamPos - WorldPos);
    vec3 reflectDir = reflect(uSunDir, normal);

    float specular = uSpecularStrength * pow(max(0.0, dot(reflectDir, viewDir)), uShininess);
    float diffuse = max(0.0, dot(normal, -uSunDir));

    // sun coloring is just white
    vec3 DirectSunLighting = vec3(1.0f) * (specular + diffuse);
    vec3 OtherLighting = vec3(uAmbientStrength); // build up the other lighting

    float intensity = 32.0f;
    // cutoff for point lights
    float radius_sqd = 64.0f;

    for (int i = 0; i < uPointLightCount; ++i) {
        vec3 toLight = uPointLightPositions[i] - WorldPos;
        float distanceToLight = length(toLight);
        vec3 pointLightDir = normalize(toLight);
        float pointDiffuse = max(0.0, dot(normal, pointLightDir));
        float attenuation = 1.0f / (1.0f + distanceToLight * distanceToLight);
        float point_brightness = 0.0f;

        if (distanceToLight * distanceToLight < radius_sqd) {
            point_brightness = pointDiffuse * attenuation * intensity;
            OtherLighting += point_brightness * uPointLightColors[i];
        }
    }

    vec3 frag_pos = FragPosLightSpace.xyz / FragPosLightSpace.w;
    frag_pos = frag_pos * 0.5 + 0.5; // remap from [-1,1] to [0,1]
    float cur_depth = frag_pos.z;
    float bias = 0.1;
    float sample_box_len = 3.0f;
    float sun_visibility = pcs_sample(frag_pos.x, frag_pos.y, cur_depth, bias, sample_box_len);
    // if its visable to sun
    vec3 final_lighting = OtherLighting + DirectSunLighting * sun_visibility;
    FragColor = texture(uAtlas, tex_coord) * vec4(final_lighting, 1.0f);
}
