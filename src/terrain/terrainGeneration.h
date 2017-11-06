#pragma once

#include "../common.h"
#include "terrain.h"
#include "water.h"

#define BIOME_PLAINS 0
#define BIOME_MOUNTAINS 1
#define BIOME_OCEAN 2
#define BIOME_UNDERGROUND 3

#define BIOMEGEN_OCEAN_LIMIT 6

void generateTerrain(Level* level);

void processChunckConditions(Terrain* terrain, SDL_Rect rect);

void processExistingLightIntensity(Terrain* terrain, SDL_Rect rect);
void processSunLightAroundBloc(Terrain* terrain, int x, int y);
void processSunLightAroundRect(Terrain* terrain, SDL_Rect rect);

void breakBloc(Terrain* terrain, WaterManager* waterManager, Uint32 x, Uint32 y);
void placeBloc(Terrain* terrain, Uint32 x, Uint32 y, Uint32 blocType);
void placeBackwall(Terrain* terrain, Uint32 x, Uint32 y, Uint32 backwallType);

