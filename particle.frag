#version 330 core

in vec2 TexCoord;
uniform sampler2D fireTexture;
uniform vec4 particleColor;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(fireTexture, TexCoord);
    FragColor = texColor * particleColor; // Apply particle color and alpha
}