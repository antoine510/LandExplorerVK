#pragma once

#include "common.h"
#include "terrain/blockTypes.h"

#define LIGHT_MASK_SIZE (2*MAX_BLOC_LIGHT -1)

typedef struct LightSource {
    Uint8 mask[LIGHT_MASK_SIZE * LIGHT_MASK_SIZE];
    Uint32 x, y;
    int xOffset, yOffset;
    Uint8 intensity;
    SDL_Color color;
} LightSource;

void updateLightSource(LightSource* light, Terrain* terrain);
//void applyLightSource(LightSource* light, Terrain* terrain, float skyBrightness);
Uint16 getMaskIndex(Uint8 x, Uint8 y);

void updateLightSources(Terrain* terrain, Uint32 x, Uint32 y);
SDL_Color getLight(LightSource* light, Uint32 x, Uint32 y);

void getLightSourcePosition(LightSource* light, Uint32* x, Uint32* y);

void addLightSource(Terrain* terrain, Uint32 x, Uint32 y, SDL_Color color, Uint8 intensity, int xOffset, int yOffset);
void addTorch(Terrain* terrain, Uint32 x, Uint32 y);
void deleteLightSource(Terrain* terrain, Uint32 x, Uint32 y);

