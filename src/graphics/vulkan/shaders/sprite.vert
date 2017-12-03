#version 450
#extension GL_ARB_separate_shader_objects : enable

// PushCst: size=56
layout(push_constant) uniform VertexPush {
	// (sx * cos, sy * -sin)
	// (sx * sin, sy *  cos)
    layout(offset = 0) mat2 sizeRot;
	// vec4(offsetX, offsetY, sizeX, sizeY)
	layout(offset = 16) vec4 texCoord;
	//layout(offset = 32) vec4 colorAlphaMod;
	layout(offset = 48) vec2 position;
	layout(offset = 56) uint layer;
} vertexPush;

layout (location = 0) in vec2 vpos;
layout (location = 0) out vec2 coordTexture;

void main()
{
    gl_Position = vec4(vertexPush.sizeRot * vpos * 2 + vertexPush.position, float(vertexPush.layer), 1.0f);

    coordTexture = vpos * vertexPush.texCoord.xy + vertexPush.texCoord.zw;
}
