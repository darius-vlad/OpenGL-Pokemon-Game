#version 410 core

out vec4 fragColor;

uniform sampler2D diffuseTexture; 
in vec2 fragTexCoord;            

void main() {
    fragColor = texture(diffuseTexture, fragTexCoord);
}