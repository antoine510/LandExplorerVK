#pragma once

#include "oglCommon.h"
#include "shader.h"

typedef struct BaseShaderUniforms {
    GLint transformLoc, colAlphaModLoc, clipOffsetLoc;

    SDL_Color* colorMod;
    float alphaMod;
	float clipx, clipy;
	GLfloat transform[6];	//	( sx*cos , -sy*sin*aspect , x )
							//	( sx*sin*aspect ,  sy*cos , y )
} BaseShaderUniforms;

BaseShaderUniforms* createBaseShaderUniforms(Shader* baseShader);

void setBaseUniformTransform(BaseShaderUniforms* uniforms, float x, float y, float xscale, float yscale);
void setBaseUniformTransformAngle(BaseShaderUniforms* uniforms, float theta);
void setBaseUniformColorAlphaMod(BaseShaderUniforms* uniforms, SDL_Color* colorMod, float alphaMod);
void setBaseUniformClipOffset(BaseShaderUniforms* uniforms, float clipx, float clipy);

void applyBaseShaderUniforms(void* structure);

void destroyBaseShaderUniforms(BaseShaderUniforms* uniforms);

