#version 330 core

layout (location = 0) in vec3 aPos;  // Vertex position
layout (location = 1) in vec2 aTexCoord; // Texture coordinate

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoord = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
