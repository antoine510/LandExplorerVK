#pragma once

#include "../common.h"
#include "texturePack.h"
#include "../terrain/terrain.h"

#define MAX_TEXTURE_STORAGE 63

struct ChunckRenderer {
    char* textureID;
    Sprite* blocTex;
    Sprite* backwallTex;
    int texturePosition[MAX_TEXTURE_STORAGE];
    int currentTextureID;

    //TextureGroup* chunckGroup;
    //ChunckBuilder* builder;

    int xC1, yC1, xC2, yC2;

    TexturePack* texPack;
    SDL_Point* viewOrigin;
    Terrain* terrain;
};

ChunckRenderer* initChunckRenderer(TexturePack* texPack, SDL_Point* viewOrigin);
void setTerrain(ChunckRenderer* chunckRenderer, Terrain* terrain);

void updateRect(ChunckRenderer* chunckRenderer, SDL_Rect rect);
void updatePos(ChunckRenderer* chunckRenderer, Uint32 x, Uint32 y);

void updateChunckRenderer(ChunckRenderer* chunckRenderer);
void renderBlocScreen(ChunckRenderer* chunckRenderer);
void renderBackwallScreen(ChunckRenderer* chunckRenderer);

void destroyChunckRenderer(ChunckRenderer* chunckRenderer);
