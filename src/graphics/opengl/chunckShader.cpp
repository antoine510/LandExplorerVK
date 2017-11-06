#include "chunckShader.h"

ChunckShaderUniforms* createChunckShaderUniforms(Shader* chunckShader, float wChunck, float hChunck)
{
    ChunckShaderUniforms* uniforms = malloc(sizeof(ChunckShaderUniforms));
    uniforms->positionLoc = glGetUniformLocation(chunckShader->programID, "position");
    uniforms->skyColorLoc = glGetUniformLocation(chunckShader->programID, "skyColor");
    uniforms->lightPosLoc = glGetUniformLocation(chunckShader->programID, "lightPos");
    uniforms->lightColorLoc = glGetUniformLocation(chunckShader->programID, "lightColor");
    uniforms->lightCountLoc = glGetUniformLocation(chunckShader->programID, "lightCount");
    uniforms->chunckSizeLoc = glGetUniformLocation(chunckShader->programID, "chunckSize");
    uniforms->clipOffsetLoc = glGetUniformLocation(chunckShader->programID, "clipOffset");

    uniforms->lightCount = 0;
    uniforms->skyColor = NULL;
    uniforms->wChunck = wChunck; uniforms->hChunck = hChunck;

    uniforms->lightsChanged = true;

    return uniforms;
}

void setChunckUniformPosition(ChunckShaderUniforms* uniforms, float x, float y) {uniforms->x = x; uniforms->y = y;}
void setChunckUniformClipOffset(ChunckShaderUniforms* uniforms, float xoff, float yoff) {uniforms->xoff = xoff; uniforms->yoff = yoff;}
void setChunckUniformColorMod(ChunckShaderUniforms* uniforms, SDL_Color* skyColor) {uniforms->skyColor = skyColor;}

void clearChunckShaderLights(ChunckShaderUniforms* uniforms)
{
    uniforms->lightCount = 0;
    uniforms->lightsChanged = true;
}

void addChunckShaderLight(ChunckShaderUniforms* uniforms, float x, float y, SDL_Color color)
{
    uniforms->lightPos[2*uniforms->lightCount] = x;
    uniforms->lightPos[2*uniforms->lightCount+1] = y;
    uniforms->lightColor[3*uniforms->lightCount] = (GLfloat)color.r / 255;
    uniforms->lightColor[3*uniforms->lightCount+1] = (GLfloat)color.g / 255;
    uniforms->lightColor[3*uniforms->lightCount+2] = (GLfloat)color.b / 255;
    uniforms->lightCount++;

    uniforms->lightsChanged = true;
}

void setChunckShaderUniforms(void* structure)
{
    ChunckShaderUniforms* uniforms = (ChunckShaderUniforms*)structure;

    glUniform2f(uniforms->positionLoc, uniforms->x, uniforms->y);
    glUniform2f(uniforms->chunckSizeLoc, uniforms->wChunck, uniforms->hChunck);
    glUniform2f(uniforms->clipOffsetLoc, uniforms->xoff, uniforms->yoff);
    glUniform3f(uniforms->skyColorLoc, (float)uniforms->skyColor->r / 255,(float)uniforms->skyColor->g / 255,(float)uniforms->skyColor->b / 255);

    if(uniforms->lightsChanged)
    {
        glUniform1ui(uniforms->lightCountLoc, uniforms->lightCount);
        glUniform2fv(uniforms->lightPosLoc, uniforms->lightCount, uniforms->lightPos);
        glUniform3fv(uniforms->lightColorLoc, uniforms->lightCount, uniforms->lightColor);
        uniforms->lightsChanged = false;
    }
}

void applyChunckUniformClipOffset(ChunckShaderUniforms* uniforms, float xoff, float yoff)
{
	uniforms->xoff = xoff; uniforms->yoff = yoff;
    glUniform2f(uniforms->clipOffsetLoc, uniforms->xoff, uniforms->yoff);
}

void applyChunckUniformPosition(ChunckShaderUniforms* uniforms, float x, float y)
{
	uniforms->x = x; uniforms->y = y;
	glUniform2f(uniforms->positionLoc, uniforms->x, uniforms->y);
}

void destroyChunckShaderUniforms(ChunckShaderUniforms* uniforms)
{
    free(uniforms);
}
