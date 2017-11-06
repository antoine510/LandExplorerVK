#include "baseShader.h"
#include "../displayInfo.h"

BaseShaderUniforms* createBaseShaderUniforms(Shader* baseShader)
{
    BaseShaderUniforms* uniforms = calloc(1, sizeof(BaseShaderUniforms));

    uniforms->transformLoc = glGetUniformLocation(baseShader->programID, "transform");
	uniforms->clipOffsetLoc = glGetUniformLocation(baseShader->programID, "clipOffset");
    uniforms->colAlphaModLoc = glGetUniformLocation(baseShader->programID, "colorAlphaMod");

    uniforms->colorMod = NULL;
    uniforms->alphaMod = 1.0f;

    return uniforms;
}

// theta in degrees
void setBaseUniformTransformAngle(BaseShaderUniforms* uniforms, float theta)
{
	float cosine = cosf(theta * 0.01745329252f), sine = sinf(theta * 0.01745329252f);

	uniforms->transform[1] = uniforms->transform[0] * sine * myDisplayMode->w / myDisplayMode->h;
	uniforms->transform[2] = uniforms->transform[3] * -sine * myDisplayMode->h / myDisplayMode->w;
	uniforms->transform[0] *= cosine;
	uniforms->transform[3] *= cosine;
}

void setBaseUniformTransform(BaseShaderUniforms* uniforms, float x, float y, float xscale, float yscale) {
	uniforms->transform[0] = xscale;
	uniforms->transform[1] = 0.0f;
	uniforms->transform[2] = 0.0f;
	uniforms->transform[3] = yscale;
	uniforms->transform[4] = x;
	uniforms->transform[5] = y;
}

void setBaseUniformColorAlphaMod(BaseShaderUniforms* uniforms, SDL_Color* colorMod, float alphaMod) { uniforms->colorMod = colorMod; uniforms->alphaMod = alphaMod; }
void setBaseUniformClipOffset(BaseShaderUniforms* uniforms, float clipx, float clipy) { uniforms->clipx = clipx; uniforms->clipy = clipy; }

void applyBaseShaderUniforms(void* structure)
{
    BaseShaderUniforms* uniforms = (BaseShaderUniforms*)structure;

    glUniformMatrix3x2fv(uniforms->transformLoc, 1, GL_FALSE, uniforms->transform);
	glUniform2f(uniforms->clipOffsetLoc, uniforms->clipx, uniforms->clipy);
    if(uniforms->colorMod == NULL)
        glUniform4f(uniforms->colAlphaModLoc, 1.0f, 1.0f, 1.0f, uniforms->alphaMod);
    else
        glUniform4f(uniforms->colAlphaModLoc,(float)uniforms->colorMod->r / 255,
                                             (float)uniforms->colorMod->g / 255,
                                             (float)uniforms->colorMod->b / 255,
                                                    uniforms->alphaMod);
}

void destroyBaseShaderUniforms(BaseShaderUniforms* uniforms)
{
    free(uniforms);
}

