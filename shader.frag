#version 330 core

// Input color interpolated from the vertex shader
in vec4 color;

// Output color written to the framebuffer
out vec4 FragColor;

void main()
{
    // Output the interpolated color
    FragColor = color;
}
