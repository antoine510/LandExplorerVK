#include "terrain.h"
#include "blockTypes.h"
#include "terrainGeneration.h"
#include "graphics/chunkRenderer.h"
#include "utility/mathUtility.h"

/**GLOBAL**/
ChunckRenderer* chunckRenderer;

static Uint16 terrainWidth, terrainHeight, terrainWidthChunck, terrainHeightChunck;

static Uint32 getBlockIndex(Uint32 x, Uint32 y);

static Uint8 chunckZeroTest[CHUNCK_WIDTH * CHUNCK_HEIGHT];

Terrain* initTerrain() {
	Terrain* terrain = (Terrain*)calloc(1, sizeof(Terrain));
	if(terrain == NULL) { printf("Error while initialiazing terrain"); }

	terrain->width = TERRAIN_WIDTH; terrain->height = TERRAIN_HEIGHT;

	terrain->widthChunck = terrain->width / CHUNCK_WIDTH; terrain->heightChunck = terrain->height / CHUNCK_HEIGHT;
	terrainWidth = terrain->width; terrainHeight = terrain->height;
	terrainWidthChunck = terrain->widthChunck; terrainHeightChunck = terrain->heightChunck;

	terrain->blocks = (Bloc*)calloc(terrain->width * terrain->height, sizeof(Bloc));
	terrain->backwall = (Bloc*)calloc(terrain->width * terrain->height, sizeof(Bloc));
	terrain->chuncks = (Chunck*)calloc(terrain->widthChunck * terrain->heightChunck, sizeof(Chunck));
	memset(chunckZeroTest, 0, CHUNCK_WIDTH * CHUNCK_HEIGHT * sizeof(Uint8));

	initBlockTypes(terrain->blocTypes);

	//loadTerrain(terrain);

	return terrain;
}

void saveTerrain(Terrain* terrain) {
	FILE *f;
	f = fopen("save.ter", "wb");
	fwrite(terrain->blocks, sizeof(Uint32), terrain->height * terrain->width, f);
	fwrite(terrain->backwall, sizeof(Uint32), terrain->height * terrain->width, f);
	fclose(f);
}

void loadTerrain(Terrain* terrain) {
	FILE *f;
	f = fopen("save.ter", "rb");
	fread(terrain->blocks, sizeof(Uint32), terrain->height * terrain->width, f);
	fread(terrain->backwall, sizeof(Uint32), terrain->height * terrain->width, f);
	fclose(f);
}

void destroyTerrain(Terrain* terrain) {
	//saveTerrain(terrain);
	destroyChuncks(terrain);
	free(terrain->blocks);
	free(terrain->backwall);
	free(terrain);
}

Uint32 getBlockIndex(Uint32 x, Uint32 y) { return x + y * terrainWidth; }

Bloc getBlock(Terrain* terrain, Uint32 x, Uint32 y) { return terrain->blocks[getBlockIndex(x, y)]; }
Bloc getBackwall(Terrain* terrain, Uint32 x, Uint32 y) { return terrain->backwall[getBlockIndex(x, y)]; }
Bloc* getBlockPtr(Terrain* terrain, Uint32 x, Uint32 y) { return terrain->blocks + getBlockIndex(x, y); }
Bloc* getBackwallPtr(Terrain* terrain, Uint32 x, Uint32 y) { return terrain->backwall + getBlockIndex(x, y); }

void getBlocCoordinatesFromPointer(Terrain* terrain, Bloc* bloc, Uint32* x, Uint32* y) {
	Uint32 offset = bloc - terrain->blocks;
	*x = offset%terrainWidth;
	*y = offset / terrainWidth;
}

Uint8 getBlockOpacity(Terrain* terrain, Uint32 x, Uint32 y) {
	if(getBlock(terrain, x, y).type != 0) return terrain->blocTypes[getBlock(terrain, x, y).type].opacity;
	return terrain->blocTypes[getBackwall(terrain, x, y).type + BACKWALL_TYPES_OFFSET].opacity;
}

bool isNaturalLightSource(Terrain* terrain, Uint32 x, Uint32 y) {
	return (getBlock(terrain, x, y).type == 0) && (getBackwall(terrain, x, y).type == 0);
}

void setBlock(Terrain* terrain, Uint32 x, Uint32 y, Bloc value) {
	terrain->blocks[getBlockIndex(x, y)] = value;
}

void setBackwall(Terrain* terrain, Uint32 x, Uint32 y, Bloc value) {
	terrain->backwall[getBlockIndex(x, y)] = value;
}

void setLightRect(Terrain* terrain, SDL_Rect rect, Uint8 value) {
	int i, j;
	clampWorld(terrain->width, terrain->height, &rect, 0);
	for(i = rect.x; i < rect.x + rect.w; i++) {
		for(j = rect.y; j < rect.y + rect.h; j++) {
			getBlockPtr(terrain, i, j)->light = value;
		}
	}
}

int isTerrainEmpty(Terrain* terrain, SDL_Rect rect) {
	clampWorld(terrain->width, terrain->height, &rect, TERRAIN_BORDER);
	int x, y;
	for(x = rect.x; x < rect.x + rect.w; x++) {
		for(y = rect.y; y < rect.y + rect.h; y++) {
			if(getBlock(terrain, x, y).type != 0 || getBackwall(terrain, x, y).type != 0) return 0;
		}
	}

	return 1;
}

int isTerrainChunckEmpty(Terrain* terrain, int x, int y) {
	SDL_Rect t = { x*CHUNCK_WIDTH - 1, y*CHUNCK_HEIGHT - 1, CHUNCK_WIDTH + 2, CHUNCK_HEIGHT + 2 };
	return isTerrainEmpty(terrain, t);
}

int isTerrainBlack(Terrain* terrain, SDL_Rect rect) {
	clampWorld(terrain->width, terrain->height, &rect, TERRAIN_BORDER);
	int x, y;
	for(x = rect.x; x < rect.x + rect.w; x++) {
		for(y = rect.y; y < rect.y + rect.h; y++) {
			if(getBlock(terrain, x, y).light != 0) return 0;
		}
	}

	return 1;
}

int isTerrainChunckBlack(Terrain* terrain, int x, int y) {
	SDL_Rect t = { x*CHUNCK_WIDTH - 1, y*CHUNCK_HEIGHT - 1, CHUNCK_WIDTH + 2, CHUNCK_HEIGHT + 2 };
	return isTerrainBlack(terrain, t);
}

SDL_Rect getChunckRect(SDL_Rect* blocRect) {
	return SDL_Rect{ blocRect->x / CHUNCK_WIDTH, blocRect->y / CHUNCK_HEIGHT,
			(blocRect->x + blocRect->w - 1) / CHUNCK_WIDTH + 1 - blocRect->x / CHUNCK_WIDTH,
			(blocRect->y + blocRect->h - 1) / CHUNCK_HEIGHT + 1 - blocRect->y / CHUNCK_HEIGHT };
}

int getTerrainHeight(Terrain* terrain, int x) {
	int res = TERRAIN_BORDER;
	while(getBlock(terrain, x, res).type == 0 && res < terrainHeight - TERRAIN_BORDER) res++;
	return res - 1;
}

Bloc* getBlocUp(Bloc* bloc) { return bloc - terrainWidth; }
Bloc* getBlocDown(Bloc* bloc) { return bloc + terrainWidth; }
Bloc* getBlocLeft(Bloc* bloc) { return bloc - 1; }
Bloc* getBlocRight(Bloc* bloc) { return bloc + 1; }
Chunck* getChunckUp(Chunck* chunck) { return chunck - terrainWidthChunck; }
Chunck* getChunckDown(Chunck* chunck) { return chunck + terrainWidthChunck; }
Chunck* getChunckLeft(Chunck* chunck) { return chunck - 1; }
Chunck* getChunckRight(Chunck* chunck) { return chunck + 1; }

Uint8 getBlocCustomValue(Terrain* terrain, Uint32 x, Uint32 y) {
	Uint8* customValues = terrain->chuncks[getBlocChunck(x, y)].customValues;
	return (customValues != NULL) ? customValues[getBlocIDWithinChunck(x, y)] : 0;
}

void setBlocCustomValue(Terrain* terrain, Uint32 x, Uint32 y, Uint8 customValue) {
	Uint8** customValues = &terrain->chuncks[getBlocChunck(x, y)].customValues;
	if(*customValues == NULL) {
		if(customValue == 0) return;
		*customValues = (Uint8*)calloc(CHUNCK_WIDTH * CHUNCK_HEIGHT, sizeof(Uint8));
	}
	(*customValues)[getBlocIDWithinChunck(x, y)] = customValue;
}

void checkChunckCustomValuesUnneeded(Terrain* terrain, Uint32 xChunck, Uint32 yChunck) {
	Uint8** customValues = &terrain->chuncks[getChunck(xChunck, yChunck)].customValues;
	if(memcmp(*customValues, chunckZeroTest, CHUNCK_WIDTH * CHUNCK_HEIGHT * sizeof(Uint8))) return; //Chunck has custom values

	free(*customValues);
	*customValues = NULL;
}

Uint8 getBlocEdges(Terrain* terrain, Bloc* bloc) {
	return ((getBlocLeft(bloc)->type == 0) << 3) + ((getBlocRight(bloc)->type == 0) << 2) +
		((getBlocDown(bloc)->type == 0) << 1) + (getBlocUp(bloc)->type == 0);   //E = lrdu
}

Uint8 getLiquidCustomValue(Terrain* terrain, Bloc* bloc) {
	Uint32 x, y;
	getBlocCoordinatesFromPointer(terrain, bloc, &x, &y);
	Uint8 waterLevel = getBlocCustomValue(terrain, x, y);
	if(getBlocUp(bloc)->type != 0 && waterLevel < 1) return 0;
	else return waterLevel + 8;
}

Uint32 getBlocIDWithinChunck(Uint32 x, Uint32 y) {
	return (x%CHUNCK_WIDTH) + (y%CHUNCK_HEIGHT)*CHUNCK_WIDTH;
}

//x and y are the BLOC's coordinates
Uint32 getBlocChunck(Uint32 x, Uint32 y) {
	return x / CHUNCK_WIDTH + y / CHUNCK_HEIGHT * terrainWidthChunck;
}

ChunckCoords getBlocChunckCoords(Uint32 x, Uint32 y) {
	return ChunckCoords{ x / CHUNCK_WIDTH, y / CHUNCK_WIDTH };
}

Uint32 getChunck(Uint32 x, Uint32 y) {
	return x + y * terrainWidthChunck;
}

Chunck* getChunckPtr(Terrain* terrain, Uint32 x, Uint32 y) { return &terrain->chuncks[getChunck(x, y)]; }

void destroyChuncks(Terrain* terrain) {
	int i;
	for(i = 0; i < terrain->widthChunck * terrain->heightChunck; i++) {
		free(terrain->chuncks[i].lights);
	}
	free(terrain->chuncks);
}
