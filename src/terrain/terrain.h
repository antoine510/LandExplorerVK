#pragma once

#include "common.h"
#include "lightSource.h"
#include "blockTypes.h"
#include "bloc.h"

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
	BlocType blocTypes[2 * BLOC_TYPE_COUNT];
	Uint8 blockOpacity[2 * BLOC_TYPE_COUNT];

	Uint16 width, height;
	Uint16 widthChunck, heightChunck;
};

Terrain* initTerrain();
void saveTerrain(Terrain* terrain);
void loadTerrain(Terrain* terrain);
void destroyTerrain(Terrain* terrain);

constexpr Uint32 getBlockIndex(Uint32 x, Uint32 y) { return x + y * TERRAIN_WIDTH; }
inline Bloc getBlock(Terrain* terrain, Uint32 x, Uint32 y) { return terrain->blocks[getBlockIndex(x, y)]; }
inline Bloc getBackwall(Terrain* terrain, Uint32 x, Uint32 y) { return terrain->backwall[getBlockIndex(x, y)]; }
inline Bloc* getBlockPtr(Terrain* terrain, Uint32 x, Uint32 y) { return terrain->blocks + getBlockIndex(x, y); }
inline Bloc* getBackwallPtr(Terrain* terrain, Uint32 x, Uint32 y) { return terrain->backwall + getBlockIndex(x, y); }

void getBlocCoordinatesFromPointer(Terrain* terrain, Bloc* bloc, Uint32* x, Uint32* y);
inline Uint8 getBlockOpacity(Terrain* terrain, Uint32 x, Uint32 y) {
	return getBlock(terrain, x, y).type ? terrain->blocTypes[getBlock(terrain, x, y).type].opacity : terrain->blocTypes[getBackwall(terrain, x, y).type + BACKWALL_TYPES_OFFSET].opacity;
}
inline bool isNaturalLightSource(Terrain* terrain, Uint32 x, Uint32 y) { return (getBlock(terrain, x, y).type == 0) && (getBackwall(terrain, x, y).type == 0); }

void setLightRect(Terrain* terrain, SDL_Rect rect, Uint8 value);

inline int getTerrainHeight(Terrain* terrain, int x, int yHint = TERRAIN_BORDER) {
	while(getBlock(terrain, x, yHint).type == 0 && yHint < TERRAIN_HEIGHT - TERRAIN_BORDER) yHint++;
	return yHint - 1;
}

Uint8 getBlocCustomValue(Terrain* terrain, Uint32 x, Uint32 y);
void setBlocCustomValue(Terrain* terrain, Uint32 x, Uint32 y, Uint8 customValue);
void checkChunckCustomValuesUnneeded(Terrain* terrain, Uint32 xChunck, Uint32 yChunck);
inline Uint8 getBlocEdges(Terrain* terrain, Bloc* bloc) { return (bloc->left()->type == 0) * 8 + (bloc->right()->type == 0) * 4 + (bloc->down()->type == 0) * 2 + (bloc->up()->type == 0); }
Uint8 getLiquidCustomValue(Terrain* terrain, Bloc* bloc);

inline Uint32 getBlocChunck(Uint32 x, Uint32 y) { return x / CHUNCK_SIZE + y / CHUNCK_SIZE * (TERRAIN_WIDTH / CHUNCK_SIZE); }
inline ChunckCoords getBlocChunckCoords(Uint32 x, Uint32 y) { return ChunckCoords{Uint16(x / CHUNCK_SIZE), Uint16(y / CHUNCK_SIZE)}; }

inline Uint32 getBlocIDWithinChunck(Uint32 x, Uint32 y) { return (x % CHUNCK_SIZE) + (y % CHUNCK_SIZE) * CHUNCK_SIZE; }
inline Uint32 getChunck(Uint32 x, Uint32 y) { return x + y * (TERRAIN_WIDTH / CHUNCK_SIZE); }
inline Chunck* getChunckPtr(Terrain* terrain, Uint32 x, Uint32 y) { return &terrain->chuncks[getChunck(x, y)]; }
void destroyChuncks(Terrain* terrain);

class TerrainRenderer;
extern TerrainRenderer* terrainRenderer;