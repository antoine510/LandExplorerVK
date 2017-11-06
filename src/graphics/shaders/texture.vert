#version 150 core

in vec2 in_Vertex;
in vec2 in_TexCoord0;

// (sx * cos, sy * -sin, px)
// (sx * sin, sy *  cos, py)
uniform mat3x2 transform;
// (px, py)
uniform vec2 clipOffset;

out vec2 coordTexture;


void main()
{
    gl_Position = vec4(transform * vec3(in_Vertex, 1.0), 0.0, 1.0);

    coordTexture = in_TexCoord0 + clipOffset;
}
