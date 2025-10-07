#version 330 core

// Input vertex position (vPosition)
layout (location = 0) in vec4 vPosition;

// Input vertex color (vColor)
layout (location = 1) in vec4 vColor;

// Output color passed to the fragment shader
out vec4 color;

// Uniform for any transformation matrix (Projection, ModelView)
uniform mat4 model_view_projection;

void main()
{
    // Apply the transformation matrix to the vertex position
    gl_Position = model_view_projection * vPosition;
    
    // Pass the color directly to the fragment shader
    color = vColor;
}
