#version 410 core

layout(location = 0) in vec3 position; // Vertex position
layout(location = 1) in vec3 normal;   // Vertex normal
layout(location = 2) in vec2 texCoord; // Texture coordinates

out vec3 fragNormal;
out vec2 fragTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Wave parameters
uniform float time; // Animation time
uniform float waveAmplitude; // Wave height
uniform float waveFrequency; // Wave frequency
uniform float waveSpeed;     // Wave speed

void main()
{
    vec3 newPosition = position;

    // Apply a sine wave displacement along the Y-axis
    newPosition.y += waveAmplitude * sin(waveFrequency * position.x + waveSpeed * time)
                               * cos(waveFrequency * position.z + waveSpeed * time);

    gl_Position = projection * view * model * vec4(newPosition, 1.0);
    fragNormal = mat3(transpose(inverse(model))) * normal; // Transform normal
    fragTexCoord = texCoord;
}
