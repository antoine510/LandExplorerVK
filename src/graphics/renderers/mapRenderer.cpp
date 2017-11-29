#include "mapRenderer.h"
#include "../graphics.h"
#include "utility/mathUtility.h"

static Uint32 procesMapPixel(MapRenderer* mapRenderer, Terrain* terrain, int x, int y);
static Uint32 processColorIntensity(Uint32 base, Uint8 light);
static void updateMapRect(MapRenderer* mapRenderer, Terrain* terrain, Uint32 x, Uint32 y, Uint32 x2, Uint32 y2);

void initMapRenderer(MapRenderer* mapRenderer, Graphics* gfx)
{
    mapRenderer->pixels = SDL_CreateRGBSurface(0, TERRAIN_WIDTH, TERRAIN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_Rect mapSize = {TERRAIN_BORDER, TERRAIN_BORDER, TERRAIN_WIDTH - 2*TERRAIN_BORDER, TERRAIN_HEIGHT - 2*TERRAIN_BORDER};
    SDL_FillRect(mapRenderer->pixels, &mapSize, SDL_MapRGBA(mapRenderer->pixels->format, 0, 0, 0, 255));

    mapRenderer->blockColors[BLOC_DIRT] = SDL_MapRGBA(mapRenderer->pixels->format, 151, 107, 75, 255);
    mapRenderer->blockColors[BLOC_ROCK] = SDL_MapRGBA(mapRenderer->pixels->format, 130, 130, 130, 255);
    mapRenderer->blockColors[BLOC_WATER] = SDL_MapRGBA(mapRenderer->pixels->format, 9, 61, 191, 255);
    mapRenderer->blockColors[BLOC_BRICK] = SDL_MapRGBA(mapRenderer->pixels->format, 120, 120, 120, 255);
    mapRenderer->blockColors[BLOC_WOOD] = SDL_MapRGBA(mapRenderer->pixels->format, 169, 125, 93, 255);
    mapRenderer->blockColors[BLOC_TORCH] = SDL_MapRGBA(mapRenderer->pixels->format, 255, 179, 0, 255);
    mapRenderer->blockColors[BLOC_GRASS] = SDL_MapRGBA(mapRenderer->pixels->format, 28, 216, 94, 255);
    mapRenderer->blockColors[BLOC_SAND] = SDL_MapRGBA(mapRenderer->pixels->format, 211, 198, 111, 255);
    mapRenderer->blockColors[BACKWALL_TYPES_OFFSET + BACKWALL_DIRT] = SDL_MapRGBA(mapRenderer->pixels->format, 88, 61, 46, 255);
    mapRenderer->blockColors[BACKWALL_TYPES_OFFSET + BACKWALL_ROCK] = SDL_MapRGBA(mapRenderer->pixels->format, 52, 52, 52, 255);

	mapRenderer->mapp = new Sprite(TERRAIN_WIDTH, TERRAIN_HEIGHT);
}

void initMapRendererStartPos(MapRenderer* mapRenderer, Graphics* gfx)
{
    mapRenderer->x = (gfx->viewOrigin.x - myDisplayMode.w/2 - 1) / BLOC_SIZE;
    mapRenderer->y = (gfx->viewOrigin.y - myDisplayMode.h/2 - 1) / BLOC_SIZE;
    mapRenderer->x2 = (gfx->viewOrigin.x + myDisplayMode.w/2 -1) / BLOC_SIZE;
    mapRenderer->y2 = (gfx->viewOrigin.y + myDisplayMode.h/2 -1) / BLOC_SIZE;
}

void updateMapRendering(MapRenderer* mapRenderer, Graphics* gfx, Map* mapp, Terrain* terrain)
{
    Uint32 newx = (gfx->viewOrigin.x - myDisplayMode.w / 2) / BLOC_SIZE;
    Uint32 newy = (gfx->viewOrigin.y - myDisplayMode.h / 2) / BLOC_SIZE;
    Uint32 newx2 = (gfx->viewOrigin.x + myDisplayMode.w / 2) / BLOC_SIZE;
    Uint32 newy2 = (gfx->viewOrigin.y + myDisplayMode.h / 2) / BLOC_SIZE;

    if(newx != mapRenderer->x)
    {
        Uint32 xmin = std::min(newx, mapRenderer->x), xmax = std::max(newx, mapRenderer->x);
        updateMapRect(mapRenderer, terrain, xmin, newy, xmax, newy2);
        mapRenderer->x = newx;
    }
    if(newx2 != mapRenderer->x2)
    {
        Uint32 xmin = std::min(newx2, mapRenderer->x2), xmax = std::max(newx2, mapRenderer->x2);
        updateMapRect(mapRenderer, terrain, xmin, newy, xmax, newy2);
        mapRenderer->x2 = newx2;
    }
    if(newy != mapRenderer->y)
    {
        Uint32 ymin = std::min(newy, mapRenderer->y), ymax = std::max(newy, mapRenderer->y);
        updateMapRect(mapRenderer, terrain, newx, ymin, newx2, ymax);
        mapRenderer->y = newy;
    }
    if(newy2 != mapRenderer->y2)
    {
        Uint32 ymin = std::min(newy2, mapRenderer->y2), ymax = std::max(newy2, mapRenderer->y2);
        updateMapRect(mapRenderer, terrain, newx, ymin, newx2, ymax);
        mapRenderer->y2 = newy2;
    }

    mapp->mapUpdated = 0;
}

void updateMapRect(MapRenderer* mapRenderer, Terrain* terrain, Uint32 x, Uint32 y, Uint32 x2, Uint32 y2)
{
    Uint32* pixel = (Uint32*)mapRenderer->pixels->pixels + y * mapRenderer->pixels->pitch/4 + x;
    Uint32* pixelRow = pixel;

    Uint32 i, j;
    for(j = y; j <= y2; j++)
    {
        for(i = x; i <= x2; i++)
        {
            *pixel = procesMapPixel(mapRenderer, terrain, i, j);
            pixel++;
        }
        pixelRow += mapRenderer->pixels->pitch/4;
        pixel = pixelRow;
    }
}

Uint32 procesMapPixel(MapRenderer* mapRenderer, Terrain* terrain, int x, int y)
{
    if(getBlock(terrain, x, y).type != 0)
    {
        return processColorIntensity(mapRenderer->blockColors[getBlock(terrain, x, y).type], getBlock(terrain, x, y).light);
    }
    else if(getBackwall(terrain, x, y).type != 0)
    {
        return processColorIntensity(mapRenderer->blockColors[getBackwall(terrain, x, y).type + BACKWALL_TYPES_OFFSET], getBlock(terrain, x, y).light);
    }
    else {return 0;}
}

Uint32 processColorIntensity(Uint32 base, Uint8 light)
{
    if(light == 0) return 0xff000000;
    if(light == MAX_BLOC_LIGHT) return base;

    float intensity = (float)(light+1) / (MAX_BLOC_LIGHT+1);
    Uint32 r = (Uint32)(((base & 0x00ff0000) >> 16) * intensity),
           g = (Uint32)(((base & 0x0000ff00) >> 8) * intensity),
           b = (Uint32)((base & 0x000000ff) * intensity);
    return ((r << 16) + (g << 8) + b) + (base & 0xff000000);       //Restore alpha component
}

void renderMap(MapRenderer* mapRenderer, Graphics* gfx, Map* mapp, Level* level)
{
    renderBackground(gfx->bgRenderer, gfx, BG_BIOMES);

	float scaling = (mapp->scalingFactor >= 0) ? (float)(1 << mapp->scalingFactor) : 1.0f / (float)(1 << -mapp->scalingFactor);
    if(!mapp->mapUpdated)
    {
        updateMapRect(mapRenderer, level->terrain, (gfx->viewOrigin.x - myDisplayMode.w / 2) / BLOC_SIZE, (gfx->viewOrigin.y - myDisplayMode.h / 2) / BLOC_SIZE,
			(gfx->viewOrigin.x + myDisplayMode.w / 2) / BLOC_SIZE, (gfx->viewOrigin.y + myDisplayMode.h / 2) / BLOC_SIZE);
		mapRenderer->mapp->getStagingBuffer().update(mapRenderer->pixels->pixels);
		mapRenderer->mapp->stageImage();
        mapp->panningPos.x = (int)(gfx->viewOrigin.x * scaling / BLOC_SIZE);     //Center the view on the world camera
        mapp->panningPos.y = (int)(TERRAIN_HEIGHT * scaling - gfx->viewOrigin.y * scaling / BLOC_SIZE);
        mapp->mapUpdated = 1;
    }

	mapRenderer->mapp->setScale(scaling);
	mapRenderer->mapp->setPosition(-mapp->panningPos.x, -mapp->panningPos.y);
	mapRenderer->mapp->draw(gfx->cmdBuf);

    int i;
    Uint32 target = COMP_GFXDATA + COMP_RECT;
    for(i = 0; i < ENTITY_COUNT; i++)
    {
        if((level->entities->components[i] & target) == target && gfx->texPack->texSets[level->entities->gfxData[i].texID]->mapIcon != NULL)
        {
            blitMapIcon(gfx->texPack, gfx->cmdBuf, &level->entities->gfxData[i], level->entities->rect[i], scaling, mapp->panningPos);
        }
    }
}

void destroyMapRenderer(MapRenderer* mapRenderer)
{
    SDL_FreeSurface(mapRenderer->pixels);
    delete mapRenderer->mapp;
}


