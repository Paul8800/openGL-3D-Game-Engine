#version 330 core

layout(location = 0) in vec2 inPosition;  // Position of the vertex (2D)

uniform mat4 modelViewProjection;  // The model-view-projection matrix
uniform float outlineThickness;    // Outline thickness

out vec2 fragPosition;  // The position for the fragment shader

void main()
{
    // Pass the vertex position to the fragment shader
    fragPosition = inPosition;

    // If outline thickness is non-zero, we apply an offset to the vertex
    // This is a simple technique to give an outline effect
    vec2 offset = normalize(inPosition) * outlineThickness;

    // Apply the model-view-projection matrix, and add offset for the outline
    gl_Position = modelViewProjection * vec4(inPosition + offset, 0.0, 1.0);
}

