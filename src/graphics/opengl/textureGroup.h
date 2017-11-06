#pragma once

#include "oglCommon.h"
#include "texture.h"
#include "chunckShader.h"

typedef struct TextureGroup {
    Texture** textures;
    int* textureIndices;
    int modeCount;
    int wCount, hCount;
    float wRatio, hRatio;     //Element to full texture bank size ratio
    int wTarget, hTarget;   //Drawing batch size
    float wGL, hGL;         //Element OGL size
    SDL_Color* colorMod;

    OGLRenderer* renderer;
    Shader* groupShader;
    ShaderType type;

    GLuint VBOid;
    GLuint VAOid;
} TextureGroup;

TextureGroup* createTextureGroup(OGLRenderer* renderer, int wElement, int hElement, int wCount, int hCount, int wTarget, int hTarget,
                                 int modeCount, const char* shaderName, ShaderType type);

void setTextureGroupColorMod(TextureGroup* texGroup, SDL_Color* colorMod);
void setTextureGroupTextureBank(TextureGroup* texGroup, Texture* tex, int mode);

void setTextureInGroup(TextureGroup* texGroup, int textureIndex, int x, int y);
void clearTextureGroup(TextureGroup* texGroup);

void drawTextureGroup(TextureGroup* texGroup, int x, int y, int mode);

void destroyTextureGroup(TextureGroup* texGroup);

