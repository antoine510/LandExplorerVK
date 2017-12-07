#include "mapRenderer.h"
#include "../graphics.h"
#include "utility/mathUtility.h"

static Uint32 procesMapPixel(MapRenderer* mapRenderer, Terrain* terrain, int x, int y);
static Uint32 processColorIntensity(Uint32 base, Uint8 light);
static void updateMapRect(MapRenderer* mapRenderer, Terrain* terrain, Uint32 x, Uint32 y, Uint32 x2, Uint32 y2);

static Uint32 mapBGRA(Uint8 r, Uint8 g, Uint8 b, Uint8 a) { return b + (g << 8) + (r << 16) + (a << 24); }

void initMapRenderer(MapRenderer* mapRenderer, Graphics* gfx) {
	//mapRenderer->pixels = SDL_CreateRGBSurface(0, TERRAIN_WIDTH, TERRAIN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	//SDL_Rect mapSize = {TERRAIN_BORDER, TERRAIN_BORDER, TERRAIN_WIDTH - 2 * TERRAIN_BORDER, TERRAIN_HEIGHT - 2 * TERRAIN_BORDER};
	//SDL_FillRect(mapRenderer->pixels, &mapSize, SDL_MapRGBA(mapRenderer->pixels->format, 0, 0, 0, 255));

	mapRenderer->blockColors[BLOC_DIRT] = mapBGRA(151, 107, 75, 255);
	mapRenderer->blockColors[BLOC_ROCK] = mapBGRA(130, 130, 130, 255);
	mapRenderer->blockColors[BLOC_WATER] = mapBGRA(9, 61, 191, 255);
	mapRenderer->blockColors[BLOC_BRICK] = mapBGRA(120, 120, 120, 255);
	mapRenderer->blockColors[BLOC_WOOD] = mapBGRA(169, 125, 93, 255);
	mapRenderer->blockColors[BLOC_TORCH] = mapBGRA(255, 179, 0, 255);
	mapRenderer->blockColors[BLOC_GRASS] = mapBGRA(28, 216, 94, 255);
	mapRenderer->blockColors[BLOC_SAND] = mapBGRA(211, 198, 111, 255);
	mapRenderer->blockColors[BACKWALL_TYPES_OFFSET + BACKWALL_DIRT] = mapBGRA(88, 61, 46, 255);
	mapRenderer->blockColors[BACKWALL_TYPES_OFFSET + BACKWALL_ROCK] = mapBGRA(52, 52, 52, 255);

	mapRenderer->mapp = new Sprite(TERRAIN_WIDTH, TERRAIN_HEIGHT);
	mapRenderer->buffer = new StagingBuffer(4 * TERRAIN_WIDTH * TERRAIN_HEIGHT);
	mapRenderer->mappedBuffer = (Uint32*)mapRenderer->buffer->mapMemory();

	Uint32* fillPtr = mapRenderer->mappedBuffer + TERRAIN_BORDER;
	for(int y = TERRAIN_BORDER; y < TERRAIN_HEIGHT - TERRAIN_BORDER; ++y) {
		std::fill_n(fillPtr, TERRAIN_WIDTH - 2 * TERRAIN_BORDER, mapBGRA(0, 0, 0, 255));
		fillPtr += TERRAIN_WIDTH;
	}
}

void initMapRendererStartPos(MapRenderer* mapRenderer, Graphics* gfx) {
	mapRenderer->x = Uint32(gfx->viewOrigin.x);
	mapRenderer->y = Uint32(gfx->viewOrigin.y);
	mapRenderer->x2 = Uint32(gfx->viewOrigin.x + gfx->viewOrigin.z) + 1;
	mapRenderer->y2 = Uint32(gfx->viewOrigin.y + gfx->viewOrigin.w) + 1;
}

void updateMapRendering(MapRenderer* mapRenderer, Graphics* gfx, Map* mapp, Terrain* terrain) {
	Uint32 newx = Uint32(gfx->viewOrigin.x);
	Uint32 newy = Uint32(gfx->viewOrigin.y);
	Uint32 newx2 = Uint32(gfx->viewOrigin.x + gfx->viewOrigin.z) + 1;
	Uint32 newy2 = Uint32(gfx->viewOrigin.y + gfx->viewOrigin.w) + 1;

	if(newx != mapRenderer->x) {
		Uint32 xmin = std::min(newx, mapRenderer->x), xmax = std::max(newx, mapRenderer->x);
		updateMapRect(mapRenderer, terrain, xmin, newy, xmax, newy2);
		mapRenderer->x = newx;
	}
	if(newx2 != mapRenderer->x2) {
		Uint32 xmin = std::min(newx2, mapRenderer->x2), xmax = std::max(newx2, mapRenderer->x2);
		updateMapRect(mapRenderer, terrain, xmin, newy, xmax, newy2);
		mapRenderer->x2 = newx2;
	}
	if(newy != mapRenderer->y) {
		Uint32 ymin = std::min(newy, mapRenderer->y), ymax = std::max(newy, mapRenderer->y);
		updateMapRect(mapRenderer, terrain, newx, ymin, newx2, ymax);
		mapRenderer->y = newy;
	}
	if(newy2 != mapRenderer->y2) {
		Uint32 ymin = std::min(newy2, mapRenderer->y2), ymax = std::max(newy2, mapRenderer->y2);
		updateMapRect(mapRenderer, terrain, newx, ymin, newx2, ymax);
		mapRenderer->y2 = newy2;
	}

	mapp->mapUpdated = 0;
}

void updateMapRect(MapRenderer* mapRenderer, Terrain* terrain, Uint32 x, Uint32 y, Uint32 x2, Uint32 y2) {
	//Uint32* pixel = (Uint32*)mapRenderer->buffer->mapMemory(4 * (x + y * mapRenderer->mapp->getExtent().width),
	//														4 * ((x2 - x) + (y2 - y) * mapRenderer->mapp->getExtent().width));
	Uint32* pixel = mapRenderer->mappedBuffer + x + y * mapRenderer->mapp->getExtent().width;
	Uint32* pixelRow = pixel;

	Uint32 i, j;
	for(j = y; j <= y2; j++) {
		for(i = x; i <= x2; i++) {
			*pixel = procesMapPixel(mapRenderer, terrain, i, j);
			pixel++;
		}
		pixelRow += mapRenderer->mapp->getExtent().width;
		pixel = pixelRow;
	}
}

Uint32 procesMapPixel(MapRenderer* mapRenderer, Terrain* terrain, int x, int y) {
	if(getBlock(terrain, x, y).type != 0) {
		return processColorIntensity(mapRenderer->blockColors[getBlock(terrain, x, y).type], getBlock(terrain, x, y).light);
	} else if(getBackwall(terrain, x, y).type != 0) {
		return processColorIntensity(mapRenderer->blockColors[getBackwall(terrain, x, y).type + BACKWALL_TYPES_OFFSET], getBlock(terrain, x, y).light);
	} else { return 0; }
}

Uint32 processColorIntensity(Uint32 base, Uint8 light) {
	if(light == 0) return 0xff000000;
	if(light == MAX_BLOC_LIGHT) return base;

	float intensity = (float)(light + 1) / (MAX_BLOC_LIGHT + 1);
	Uint8 r = (Uint8)(((base >> 16) & 0xff) * intensity),
		g = (Uint8)(((base >> 8) & 0xff) * intensity),
		b = (Uint8)((base & 0xff) * intensity);
	return ((r << 16) + (g << 8) + b) + (base & 0xff000000);       //Restore alpha component
}

void renderMap(MapRenderer* mapRenderer, Graphics* gfx, Map* mapp, Level* level) {
	renderBackground(gfx->bgRenderer, gfx, BG_BIOMES);

	float scaling = (mapp->scalingFactor >= 0) ? (float)(1 << mapp->scalingFactor) : 1.0f / (float)(1 << -mapp->scalingFactor);
	if(!mapp->mapUpdated) {
		updateMapRect(mapRenderer, level->terrain, Uint32(gfx->viewOrigin.x), Uint32(gfx->viewOrigin.y),
					  Uint32(gfx->viewOrigin.x + gfx->viewOrigin.z), Uint32(gfx->viewOrigin.y + gfx->viewOrigin.w));
		//mapRenderer->buffer->unmapMemory();
		mapRenderer->mapp->stageBuffer(*mapRenderer->buffer);
		//mapRenderer->mappedBuffer = (Uint32*)mapRenderer->buffer->mapMemory();
		mapp->panningPos.x = (int)((gfx->viewOrigin.x + gfx->viewOrigin.z / 2) * scaling - (myDisplayMode.w >> 1));     //Center the view on the world camera
		mapp->panningPos.y = (int)((gfx->viewOrigin.y + gfx->viewOrigin.w / 2) * scaling - (myDisplayMode.h >> 1));
		mapp->mapUpdated = 1;
	}

	mapRenderer->mapp->setScale(scaling);
	mapRenderer->mapp->setPosition(-mapp->panningPos.x, -mapp->panningPos.y);
	mapRenderer->mapp->draw(gfx->cmdBuf);

	int i;
	Uint32 target = COMP_GFXDATA + COMP_RECT;
	for(i = 0; i < ENTITY_COUNT; i++) {
		if((level->entities->components[i] & target) == target && gfx->texPack->texSets[level->entities->gfxData[i].texID]->mapIcon != NULL) {
			blitMapIcon(gfx->texPack, gfx->cmdBuf, &level->entities->gfxData[i], level->entities->rect[i], scaling, mapp->panningPos);
		}
	}
}

void destroyMapRenderer(MapRenderer* mapRenderer) {
	mapRenderer->buffer->unmapMemory();
	delete mapRenderer->buffer;
	delete mapRenderer->mapp;
}


