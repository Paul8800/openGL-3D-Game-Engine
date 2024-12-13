#version 330 core

in vec2 fragPosition;  // The position passed from the vertex shader

uniform vec3 mainColor;    // The color of the main shape
uniform vec3 outlineColor; // The color of the outline
uniform float outlineThickness;  // Outline thickness
uniform float edgeDistance;  // A small value to help determine outline borders

out vec4 fragColor;

void main()
{
    // Calculate the distance from the center of the shape (0, 0) to the fragment position
    float distanceFromCenter = length(fragPosition);

    // If the distance is less than the outline thickness, we draw the outline
    if (distanceFromCenter > (1.0 - outlineThickness) && distanceFromCenter < 1.0)
    {
        fragColor = vec4(outlineColor, 1.0);  // Apply outline color
    }
    else if (distanceFromCenter < 1.0)
    {
        fragColor = vec4(mainColor, 1.0);  // Apply main shape color
    }
    else
    {
        discard;  // Outside the shape, discard
    }
}

