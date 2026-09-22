#shader vertex
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 OtherLighting;
out vec4 FragPosLightSpace;
out vec3 DirectSunLighting;

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

    // sun coloring is just white
    DirectSunLighting = vec3(1.0f) * (specular + diffuse);
    OtherLighting = vec3(uAmbientStrength); // build up the other lighting

    float intensity = 0.85f;
    // cutoff for point lights
    float radius_sqd = 64.0f;

    for (int i = 0; i < uPointLightCount; ++i) {
        vec3 toLight = uPointLightPositions[i] - WorldPos;
        float distanceToLight = length(toLight);
        vec3 pointLightDir = normalize(toLight);
        float pointDiffuse = max(0.0, dot(aNormal, pointLightDir));
        float attenuation = 1.0f / (1.0f + distanceToLight * distanceToLight);
        float point_brightness = 0.0f;

        if (distanceToLight * distanceToLight < radius_sqd) {
            float point_brightness = pointDiffuse * attenuation * intensity;
            OtherLighting += point_brightness * uPointLightColors[i];
        }
    }
}

#shader fragment
#version 410 core

in vec2 TexCoord;
in vec3 OtherLighting;
in vec3 DirectSunLighting;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform sampler2D tex;
uniform sampler2D uShadowMap;

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
    vec3 frag_pos = FragPosLightSpace.xyz / FragPosLightSpace.w;
    frag_pos = frag_pos * 0.5 + 0.5; // remap from [-1,1] to [0,1]
    float cur_depth = frag_pos.z;
    float bias = 0.1;
    float sample_box_len = 3.0f;
    float sun_visibility = pcs_sample(frag_pos.x, frag_pos.y, cur_depth, bias, sample_box_len);
    // if its visable to sun
	FragColor = texture(tex, TexCoord) * vec4(OtherLighting + DirectSunLighting * sun_visibility, 1.0f);

}
