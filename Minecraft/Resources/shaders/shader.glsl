#shader vertex
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform float texMultiplier;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord * texMultiplier;
}

#shader fragment
#version 410 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;

void main()
{
	// Transform from [-128, 128] range to [0, 256] range
    //vec2 transformedTexCoord = TexCoord + 128.0;  // Shift from [-128, 128] to [0, 256]

	// Normalize to [0, 1] range by dividing by 256
    vec2 normalizedTexCoord = TexCoord / 128.0f;

    // Clamp to ensure it stays within the [0, 1] range, in case coordinates are outside bounds
    normalizedTexCoord = clamp(normalizedTexCoord, 0.0, 1.0);

	FragColor = texture(tex, normalizedTexCoord);
}
