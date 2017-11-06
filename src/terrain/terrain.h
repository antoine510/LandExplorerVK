#pragma once

#include "common.h"
#include "lightSource.h"
#include "blockTypes.h"
#include "bloc.h"

#define TERRAIN_WIDTH 1024
#define TERRAIN_HEIGHT 1024

#define TERRAIN_BORDER 16
#define TERRAIN_BORDER_CHUNCK 1

#define CHUNCK_WIDTH 16
#define CHUNCK_HEIGHT 16

#define CHUNCK_WIDTH_PX (CHUNCK_WIDTH * BLOC_SIZE)
#define CHUNCK_HEIGHT_PX (CHUNCK_HEIGHT * BLOC_SIZE)

#define BLOC_NONE 0
#define BLOC_DIRT 1
#define BLOC_ROCK 2
#define BLOC_WATER 3
#define BLOC_BRICK 4
#define BLOC_WOOD 5
#define BLOC_TORCH 6
#define BLOC_GRASS 7
#define BLOC_SAND 8

#define BACKWALL_NONE 0
#define BACKWALL_DIRT 1
#define BACKWALL_ROCK 2

typedef struct Chunck {
    LightSource* lights;
    Uint8 lightCount;
    Uint8* customValues;

    Uint8 biome;
    bool black : 1;
    bool empty : 1;
} Chunck;

typedef struct ChunckCoords {
    Uint16 x, y;
} ChunckCoords;

struct Terrain {
    Bloc* blocks;
    Bloc* backwall;
    Chunck* chuncks;
    BlocType blocTypes[2*BLOC_TYPE_COUNT];
    Uint8 blockOpacity[2*BLOC_TYPE_COUNT];

    Uint16 width, height;
    Uint16 widthChunck, heightChunck;
};

Terrain* initTerrain();
void saveTerrain(Terrain* terrain);
void loadTerrain(Terrain* terrain);
void destroyTerrain(Terrain* terrain);

Bloc getBlock(Terrain* terrain, Uint32 x, Uint32 y);
Bloc getBackwall(Terrain* terrain, Uint32 x, Uint32 y);
Bloc* getBlockPtr(Terrain* terrain, Uint32 x, Uint32 y);
Bloc* getBackwallPtr(Terrain* terrain, Uint32 x, Uint32 y);
void getBlocCoordinatesFromPointer(Terrain* terrain, Bloc* bloc, Uint32* x, Uint32* y);
Uint8 getBlockOpacity(Terrain* terrain, Uint32 x, Uint32 y);
bool isNaturalLightSource(Terrain* terrain, Uint32 x, Uint32 y);

void setBlock(Terrain* terrain, Uint32 x, Uint32 y, Bloc value);
void setBackwall(Terrain* terrain, Uint32 x, Uint32 y, Bloc value);
void setLightRect(Terrain* terrain, SDL_Rect rect, Uint8 value);

int isTerrainEmpty(Terrain* terrain, SDL_Rect rect);
int isTerrainChunckEmpty(Terrain* terrain, int x, int y);
int isTerrainBlack(Terrain* terrain, SDL_Rect rect);
int isTerrainChunckBlack(Terrain* terrain, int x, int y);
int getTerrainHeight(Terrain* terrain, int x);
SDL_Rect getChunckRect(SDL_Rect* blocRect);

Bloc* getBlocUp(Bloc* bloc);
Bloc* getBlocDown(Bloc* bloc);
Bloc* getBlocLeft(Bloc* bloc);
Bloc* getBlocRight(Bloc* bloc);
Chunck* getChunckUp(Chunck* chunck);
Chunck* getChunckDown(Chunck* chunck);
Chunck* getChunckLeft(Chunck* chunck);
Chunck* getChunckRight(Chunck* chunck);

Uint8 getBlocCustomValue(Terrain* terrain, Uint32 x, Uint32 y);
void setBlocCustomValue(Terrain* terrain, Uint32 x, Uint32 y, Uint8 customValue);
void checkChunckCustomValuesUnneeded(Terrain* terrain, Uint32 xChunck, Uint32 yChunck);
Uint8 getBlocEdges(Terrain* terrain, Bloc* bloc);
Uint8 getLiquidCustomValue(Terrain* terrain, Bloc* bloc);

Uint32 getBlocIDWithinChunck(Uint32 x, Uint32 y);
Uint32 getBlocChunck(Uint32 x, Uint32 y);   //x and y are the BLOC's coordinates
ChunckCoords getBlocChunckCoords(Uint32 x, Uint32 y);
Uint32 getChunck(Uint32 x, Uint32 y);
Chunck* getChunckPtr(Terrain* terrain, Uint32 x, Uint32 y);
void destroyChuncks(Terrain* terrain);

extern ChunckRenderer* chunckRenderer;  //Global var

