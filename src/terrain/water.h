#pragma once

#include "../common.h"
#include "terrain.h"

#define WSPS 2

#define WATERMAN_MAX_ACTIVE_CHUNCKS 256

typedef struct WaterManager {
    ChunckCoords activeChuncks[WATERMAN_MAX_ACTIVE_CHUNCKS], nextActiveChuncks[WATERMAN_MAX_ACTIVE_CHUNCKS];
    int activeChunckCount, nextActiveChunckCount;
    Uint32 currentChunck;
    Uint8 currentLine;
} WaterManager;

WaterManager* waterman_create();

void waterman_update(WaterManager* waterman, Terrain* terrain);
void waterman_addActiveBloc(WaterManager* waterman, Uint32 x, Uint32 y);

void waterman_destroy(WaterManager* waterman);

