#pragma once

#include "../common.h"
#include "terrain.h"

#define MAX_MODULE_FILENAME_LENGTH 128
#define MAX_UNITS_PER_MODULE 32

typedef struct Module {
    SDL_Point size, ref;
    SDL_Point guardPos[MAX_UNITS_PER_MODULE], archerPos[MAX_UNITS_PER_MODULE];
    int archerCount, guardCount;
    Uint8* blocks;
    Uint8* backwall;
} Module;


void destroyModule(Module* module);
Module* getModuleFromTerrain(Terrain* terrain, SDL_Rect rect);
void module_setEnemys(Module* module, int archerCount, int guardCount, SDL_Point* archerPos, SDL_Point* guardPos);
void pasteModule(Module* module, Level* level, SDL_Point point, bool processConditions, bool mirror);
void saveModule(Module* module, const char* moduleName);
Module* loadModule(const char* moduleName);

