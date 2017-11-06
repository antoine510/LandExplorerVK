#version 150 core

in vec2 coordTexture;

uniform sampler2D mytexture;
uniform vec4 colorAlphaMod;

out vec4 out_Color;

void main()
{
    out_Color = texture(mytexture, coordTexture) * colorAlphaMod;
}
