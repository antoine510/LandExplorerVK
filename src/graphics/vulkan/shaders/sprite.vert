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
} vertexPush;

layout (location = 0) in vec2 vpos;
layout (location = 0) out vec2 coordTexture;

void main()
{
    gl_Position = vec4(vertexPush.sizeRot * vpos * 2 + vertexPush.position, 0.0, 1.0);

    coordTexture = vpos * vertexPush.texCoord.xy + vertexPush.texCoord.zw;
}
