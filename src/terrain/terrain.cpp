#include "terrain.h"
#include "blockTypes.h"
#include "terrainGeneration.h"
#include "utility/mathUtility.h"


static Uint16 terrainWidth, terrainHeight, terrainWidthChunck, terrainHeightChunck;

static Uint8 chunckZeroTest[CHUNCK_BLOCS];

Terrain* initTerrain() {
	Terrain* terrain = (Terrain*)calloc(1, sizeof(Terrain));
	if(terrain == NULL) { printf("Error while initialiazing terrain"); }

	terrain->width = TERRAIN_WIDTH; terrain->height = TERRAIN_HEIGHT;

	terrain->widthChunck = terrain->width / CHUNCK_SIZE; terrain->heightChunck = terrain->height / CHUNCK_SIZE;
	terrainWidth = terrain->width; terrainHeight = terrain->height;
	terrainWidthChunck = terrain->widthChunck; terrainHeightChunck = terrain->heightChunck;

	terrain->blocks = (Bloc*)calloc(terrain->width * terrain->height, sizeof(Bloc));
	terrain->backwall = (Bloc*)calloc(terrain->width * terrain->height, sizeof(Bloc));
	terrain->chuncks = (Chunck*)calloc(terrain->widthChunck * terrain->heightChunck, sizeof(Chunck));
	memset(chunckZeroTest, 0, CHUNCK_BLOCS * sizeof(Uint8));

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

void getBlocCoordinatesFromPointer(Terrain* terrain, Bloc* bloc, Uint32* x, Uint32* y) {
	Uint32 offset = Uint32(bloc - terrain->blocks);
	*x = offset % terrainWidth;
	*y = offset / terrainWidth;
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

Uint8 getBlocCustomValue(Terrain* terrain, Uint32 x, Uint32 y) {
	Uint8* customValues = terrain->chuncks[getBlocChunck(x, y)].customValues;
	return (customValues != NULL) ? customValues[getBlocIDWithinChunck(x, y)] : 0;
}

void setBlocCustomValue(Terrain* terrain, Uint32 x, Uint32 y, Uint8 customValue) {
	Uint8** customValues = &terrain->chuncks[getBlocChunck(x, y)].customValues;
	if(*customValues == NULL) {
		if(customValue == 0) return;
		*customValues = (Uint8*)calloc(CHUNCK_BLOCS, sizeof(Uint8));
	}
	(*customValues)[getBlocIDWithinChunck(x, y)] = customValue;
}

void checkChunckCustomValuesUnneeded(Terrain* terrain, Uint32 xChunck, Uint32 yChunck) {
	Uint8** customValues = &terrain->chuncks[getChunck(xChunck, yChunck)].customValues;
	if(memcmp(*customValues, chunckZeroTest, CHUNCK_BLOCS * sizeof(Uint8))) return; //Chunck has custom values

	free(*customValues);
	*customValues = NULL;
}

Uint8 getLiquidCustomValue(Terrain* terrain, Bloc* bloc) {
	Uint32 x, y;
	getBlocCoordinatesFromPointer(terrain, bloc, &x, &y);
	Uint8 waterLevel = getBlocCustomValue(terrain, x, y);
	if(bloc->up()->type != 0 && waterLevel < 1) return 0;
	else return waterLevel + 8;
}

void destroyChuncks(Terrain* terrain) {
	int i;
	for(i = 0; i < terrain->widthChunck * terrain->heightChunck; i++) {
		free(terrain->chuncks[i].lights);
	}
	free(terrain->chuncks);
}
