#pragma once

#include "common.h"
#include "terrain/terrain.h"
#include "entities.h"
#include "terrain/water.h"
#include "physics/sweepAndPrune.h"
#include "lightSource.h"
#include "player/playerController.h"
#include "keyStates.h"
#include "modeUpdateResult.h"

struct Level {
    Terrain* terrain;
    Entities* entities;
    PlayerControl* playerControl;
    WaterManager* waterManager;

    int playerID;

    Vec2 spawnPoint;

    float timeMinutes;
    SDL_Color skyColor;

    bool finished;
};

Level* initLevel(Vec4* viewOrigin);

ModeUpdateResult updateLevel(Level* level, KeyStates* keyStates);
SDL_Color getSkyColor(float timeMinutes);

void destroyLevel(Level* level);

