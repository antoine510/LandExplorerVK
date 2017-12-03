#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 1) uniform sampler2D blocSampler;

layout(push_constant) uniform FragPush {
	layout(offset = 16) vec3 skyColor;
} fragPush;

layout (location = 0) in vec2 inUV;
layout (location = 1) in float intensity;
layout (location = 0) out vec4 outColor;

void main() {
	vec4 tex = texture(blocSampler, inUV);
	outColor = vec4(tex.rgb * fragPush.skyColor * intensity, tex.a);
}