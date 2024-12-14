
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform vec3 color;
uniform vec3 outlineColor;
uniform float outlineThickness;

void main() {
    float edgeThreshold = 0.5 - outlineThickness;

    if (TexCoord.x < outlineThickness || TexCoord.x > 1.0 - outlineThickness || 
        TexCoord.y < outlineThickness || TexCoord.y > 1.0 - outlineThickness) {
        FragColor = vec4(outlineColor, 1.0); // Outline color
    } else {
        FragColor = vec4(color, 1.0); // Main color
    }
}

