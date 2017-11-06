#version 150 core


in vec2 in_Vertex;
in vec2 in_TexCoord0;


uniform vec2 position;
uniform vec2 chunckSize;
uniform vec2 clipOffset;


out vec2 coordPixel;
out vec2 coordTexture;


void main()
{
    gl_Position = vec4(in_Vertex + position, 0.0, 1.0);

	coordPixel.x = (in_Vertex.x +1.0)/chunckSize.x;
	coordPixel.y = (-in_Vertex.y +1.0)/chunckSize.y;
    coordTexture = in_TexCoord0 + clipOffset;
}
