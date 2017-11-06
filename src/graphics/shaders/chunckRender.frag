#version 150 core


in vec2 coordPixel;
in vec2 coordTexture;


uniform sampler2D mytexture;
uniform vec3 skyColor;
uniform vec2 lightPos[256];
uniform vec3 lightColor[256];
uniform uint lightCount;


out vec4 out_Color;


void main()
{
	vec4 texColor = texture(mytexture, coordTexture);
	vec3 colorMod = skyColor;
	int i;
	for(i = 0; i < int(lightCount); i++)	{
		float dst = distance(lightPos[i], coordPixel);
		if(dst < 0.5)
		{
			vec3 lightMod = (1.0 - 2*dst) * lightColor[i];
			colorMod = max(colorMod, lightMod);
		}
	}
    out_Color = vec4(colorMod.r * texColor.r, 
					 colorMod.g * texColor.g, 
					 colorMod.b * texColor.b, texColor.a);
}
