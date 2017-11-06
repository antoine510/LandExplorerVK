#pragma once

#include "oglCommon.h"
#include "shader.h"

#define MAX_LIGHT_COUNT 256

typedef struct ChunckShaderUniforms {
    GLint positionLoc, skyColorLoc, lightPosLoc, lightColorLoc, lightCountLoc, chunckSizeLoc, clipOffsetLoc;
    SDL_Color* skyColor;

    GLfloat lightPos[2*MAX_LIGHT_COUNT];
    GLfloat lightColor[3*MAX_LIGHT_COUNT];
    unsigned int lightCount;

    float wChunck, hChunck;
    float xoff, yoff;
    float x, y;

    bool lightsChanged;
} ChunckShaderUniforms;

ChunckShaderUniforms* createChunckShaderUniforms(Shader* chunckShader, float wChunck, float hChunck);

void setChunckUniformPosition(ChunckShaderUniforms* uniforms, float x, float y);
void setChunckUniformClipOffset(ChunckShaderUniforms* uniforms, float xoff, float yoff);
void setChunckUniformColorMod(ChunckShaderUniforms* uniforms, SDL_Color* skyColor);

void clearChunckShaderLights(ChunckShaderUniforms* uniforms);
void addChunckShaderLight(ChunckShaderUniforms* uniforms, float x, float y, SDL_Color color);

void setChunckShaderUniforms(void* structure);
void applyChunckUniformClipOffset(ChunckShaderUniforms* uniforms, float xoff, float yoff);
void applyChunckUniformPosition(ChunckShaderUniforms* uniforms, float x, float y);

void destroyChunckShaderUniforms(ChunckShaderUniforms* uniforms);

