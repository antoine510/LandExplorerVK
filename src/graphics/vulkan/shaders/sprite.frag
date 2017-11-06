#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform FragmentPush {
	layout(offset = 32) vec4 colorAlphaMod;
} fragPush;

layout (binding = 0) uniform sampler2D mytexture;

layout (location = 0) in vec2 coordTexture;
layout (location = 0) out vec4 out_Color;

void main()
{
    out_Color = texture(mytexture, coordTexture) * fragPush.colorAlphaMod;
}
