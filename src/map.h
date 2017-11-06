#pragma once

#include "common.h"
#include "keyStates.h"
#include "entities.h"
#include "modeUpdateResult.h"

typedef struct Map {
    int scalingFactor;
    Uint8 panning;
    SDL_Point startPanningPos;
    SDL_Point panningPos;
    int mapUpdated;
} Map;

Map* initMap();

ModeUpdateResult updateMap(Map* mapp, KeyStates* keyStates);
void scaleMap(Map* mapp, int deltaScaling, int centerX, int centerY);

void destroyMap(Map* mapp);

