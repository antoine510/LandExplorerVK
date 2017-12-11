#include "terrainGeneration.h"
#include "blockTypes.h"
#include <cstring>
#include "graphics/renderers/terrainRenderer.h"
#include "caves.h"
#include "heightProfile.h"
#include "buildingGen.h"
#include "blockTypes.h"
#include "level.h"
#include "utility/mathUtility.h"
#include "utility/random.h"
#include "sound.h"

static void generateOcean(Terrain* terrain, int start, int oceanLength, int beachLength, bool orientation);

static void propagateSunLight(Terrain* terrain, int x, int y);
static void processBlockConditions(Terrain* terrain, SDL_Rect rect);

void generateTerrain(Level* level)
{
    Terrain* terrain = level->terrain;
    int i, j;  //Borders
    for(i = 0; i < terrain->width-1; i++)
        getBlockPtr(terrain, i, TERRAIN_BORDER-1)->type = 1;
    for(i = 0; i < terrain->width-1; i++)
        getBlockPtr(terrain, i, terrain->height-TERRAIN_BORDER)->type = 1;
    for(i = 0; i < terrain->height-1; i++)
        getBlockPtr(terrain, TERRAIN_BORDER-1, i)->type = 1;
    for(i = 0; i < terrain->height-1; i++)
        getBlockPtr(terrain, terrain->width-TERRAIN_BORDER, i)->type = 1;

    #ifdef PERFLOG
    int t = SDL_GetTicks();
    #endif // PERFLOG

    PerlinNoise perlin = initPerlin(terrain->width, 8);
    PerlinNoise perlin2 = initPerlin(terrain->width, 25);
    PerlinNoise perlin3 = initPerlin(terrain->width, 110);
    HeightProfile hProfile;
    generateHeightProfile(&hProfile, terrain);

    for(i = TERRAIN_BORDER; i < terrain->width - TERRAIN_BORDER; i++)
    {
        float p = perlinNoise(perlin, i), p2 = perlinNoise(perlin2, i), p3 = perlinNoise(perlin3, i);
        int groundHeight = (int)(p*hProfile.lowScale[i] + p2*hProfile.mediumScale[i] + p3*hProfile.highScale[i]);
        int dirtLimit = (int)(OCEAN_HEIGHT*terrain->height) - groundHeight;
        int rockLimit = (int)(OCEAN_HEIGHT*terrain->height) - groundHeight +20;
        getBlockPtr(terrain, i, dirtLimit)->type = 7;
        for(j = dirtLimit+1; j < rockLimit; j++)
        {
            getBlockPtr(terrain, i, j)->type = BLOC_DIRT;
            getBackwallPtr(terrain, i, j+4)->type = BACKWALL_DIRT;
        }
        for(j = rockLimit; j <= TERRAINGEN_UNDERGROUND_LIMIT*terrain->height; j++)
        {
            getBlockPtr(terrain, i, j)->type = BLOC_ROCK;
            getBackwallPtr(terrain, i, j)->type = BACKWALL_ROCK;
        }
        for(j = (int)(TERRAINGEN_UNDERGROUND_LIMIT*terrain->height) +1; j < terrain->height - TERRAIN_BORDER; j++)
        {
            getBlockPtr(terrain, i, j)->type = 2;
        }
    }

    destroyHeightProfile(&hProfile);

    for(j = TERRAIN_BORDER_CHUNCK; j < terrain->heightChunck - TERRAIN_BORDER_CHUNCK; j++)
    {
        for(i = TERRAIN_BORDER_CHUNCK; i < BIOMEGEN_OCEAN_LIMIT; i++)
        {
            getChunckPtr(terrain, i, j)->biome = BIOME_OCEAN;
        }
        for(i = BIOMEGEN_OCEAN_LIMIT; i < (PLAINS_LIMIT + PLAINS_TRANSITION_LENGTH) / CHUNCK_SIZE; i++)
        {
            getChunckPtr(terrain, i, j)->biome = BIOME_PLAINS;
        }
        for(i = (PLAINS_LIMIT + PLAINS_TRANSITION_LENGTH) / CHUNCK_SIZE; i < terrain->widthChunck - BIOMEGEN_OCEAN_LIMIT; i++)
        {
            getChunckPtr(terrain, i, j)->biome = BIOME_MOUNTAINS;
        }
        for(i = terrain->widthChunck - BIOMEGEN_OCEAN_LIMIT; i < terrain->widthChunck - TERRAIN_BORDER_CHUNCK; i++)
        {
            getChunckPtr(terrain, i, j)->biome = BIOME_OCEAN;
        }
    }
    for(i = TERRAIN_BORDER_CHUNCK; i < terrain->widthChunck; i++)
    {
        for(j = (int)(TERRAINGEN_UNDERGROUND_LIMIT*terrain->heightChunck) -1; j < terrain->heightChunck - TERRAIN_BORDER_CHUNCK; j++)
        {
            getChunckPtr(terrain, i, j)->biome = BIOME_UNDERGROUND;
        }
    }

    generateCaves(terrain);
    generateOcean(terrain, TERRAIN_BORDER, 50, 30, false);
    generateOcean(terrain, terrain->width - TERRAIN_BORDER - 80, 50, 30, true);

    generateBuildings(level);

    #ifdef PERFLOG
    printf("Terrain generation : %d\n", SDL_GetTicks() - t);
    t = SDL_GetTicks();
    #endif // PERFLOG

    SDL_Rect allTerrain = {0, 0, terrain->width, terrain->height};        //Optimization : process only non-empty chuncks
    clampWorld(terrain->width, terrain->height, &allTerrain, TERRAIN_BORDER);
    processBlockConditions(terrain, allTerrain);

    #ifdef PERFLOG
    printf("Terrain processing : %d\n", SDL_GetTicks() - t);
    #endif // PERFLOG
}

void generateOcean(Terrain* terrain, int start, int oceanLength, int beachLength, bool orientation)
{
    int beachStart, oceanStart;
    if(!orientation)
        {oceanStart = start; beachStart = start + oceanLength;}
    else
        {oceanStart = start+beachLength; beachStart = start;}

    int x, y;
    for(x = oceanStart; x <= oceanStart+oceanLength; x++)
    {
        int xProd = orientation ? (x - oceanStart - oceanLength) : x - oceanStart;
        int oceanHeight = 20 - (int)(20.0f * xProd * xProd / (oceanLength * oceanLength));

        for(y = TERRAIN_BORDER; y < (int)(OCEAN_HEIGHT*terrain->height); y++)
        {getBlockPtr(terrain, x, y)->type = BLOC_NONE; getBackwallPtr(terrain, x, y)->type = BACKWALL_NONE;}
        for(y = (int)(OCEAN_HEIGHT*terrain->height); y < (int)(OCEAN_HEIGHT*terrain->height) + oceanHeight; y++)
        {getBlockPtr(terrain, x, y)->type = BLOC_WATER; getBackwallPtr(terrain, x, y)->type = BACKWALL_NONE;}
        for(y = (int)(OCEAN_HEIGHT*terrain->height) + oceanHeight; y < (int)(OCEAN_HEIGHT*terrain->height) + oceanHeight + 18; y++)
        {getBlockPtr(terrain, x, y)->type = BLOC_SAND; getBackwallPtr(terrain, x, y)->type = BACKWALL_NONE;}

        setBlocCustomValue(terrain, x, (int)(OCEAN_HEIGHT*terrain->height), 7);    //Water level on top of the ocean
    }
    for(x = beachStart; x <= beachStart+beachLength; x++)
    {
        int xProd = orientation ? (x - beachStart - beachLength) : x - beachStart;
        int sandHeight = 18 - (int)(18.0f * xProd * xProd / (beachLength * beachLength));

        int groundHeight = getTerrainHeight(terrain, x);
        for(y = groundHeight +1; y < groundHeight + sandHeight; y++)
        {getBlockPtr(terrain, x, y)->type = BLOC_SAND; getBackwallPtr(terrain, x, y)->type = BACKWALL_NONE;}
    }
}

void processBlockConditions(Terrain* terrain, SDL_Rect rect)
{
    int i, j;
    for(i = rect.x; i < rect.x + rect.w; i++)
    {
        for(j = rect.y; j < rect.y + rect.h && j < TERRAINGEN_UNDERGROUND_LIMIT*terrain->height; j++)
        {
            if(!isNaturalLightSource(terrain, i, j) &&
               (isNaturalLightSource(terrain, i, j-1) ||
                isNaturalLightSource(terrain, i-1, j) ||       //We have a non-illuminated block adjacent to
                isNaturalLightSource(terrain, i+1, j) ||       //a natural light source
                isNaturalLightSource(terrain, i, j+1)))
            {
                getBlockPtr(terrain, i, j)->light = MAX_BLOC_LIGHT;    //We set first block to max intensity
                propagateSunLight(terrain, i, j);
            }
        }
    }
}

void processExistingLightIntensity(Terrain* terrain, SDL_Rect rect)
{
    int i, j;
    clampWorld(terrain->width, terrain->height, &rect, TERRAIN_BORDER);
    for(i = rect.x; i < rect.x + rect.w; i++)
    {
        for(j = rect.y; j < rect.y + rect.h; j++)
        {
            if((j < TERRAINGEN_UNDERGROUND_LIMIT*terrain->height &&
              (isNaturalLightSource(terrain, i, j-1) ||
               isNaturalLightSource(terrain, i-1, j) ||       //We have a non-illuminated block adjacent to
               isNaturalLightSource(terrain, i+1, j) ||       //a natural light source or torch
               isNaturalLightSource(terrain, i, j+1))) ||
               getBlock(terrain, i, j).type == 6)
            {
                getBlockPtr(terrain, i, j)->light = MAX_BLOC_LIGHT;    //We set bloc to max intensity
            }
            if(getBlock(terrain, i, j).light > 1) propagateSunLight(terrain, i, j);
        }
    }
}

void processSunLightAroundBloc(Terrain* terrain, int x, int y)
{
    SDL_Rect black = {x-MAX_BLOC_LIGHT, y-MAX_BLOC_LIGHT, 2*MAX_BLOC_LIGHT+1, 2*MAX_BLOC_LIGHT+1};
    SDL_Rect reprocess = {x-MAX_BLOC_LIGHT-1, y-MAX_BLOC_LIGHT-1, 2*MAX_BLOC_LIGHT+3, 2*MAX_BLOC_LIGHT+3};
    setLightRect(terrain, black, 0);
    processExistingLightIntensity(terrain, reprocess);
}

void processSunLightAroundRect(Terrain* terrain, SDL_Rect rect)
{
    SDL_Rect black = {rect.x-MAX_BLOC_LIGHT, rect.y-MAX_BLOC_LIGHT, 2*MAX_BLOC_LIGHT + rect.w, 2*MAX_BLOC_LIGHT + rect.h};
    SDL_Rect reprocess = {rect.x-MAX_BLOC_LIGHT-1, rect.y-MAX_BLOC_LIGHT-1, 2*MAX_BLOC_LIGHT + rect.w+2, 2*MAX_BLOC_LIGHT + rect.h+2};
    setLightRect(terrain, black, 0);
    processExistingLightIntensity(terrain, reprocess);

    //updateRect(chunckRenderer, black);  //Graphical update
}

void propagateSunLight(Terrain* terrain, int x, int y)
{
    //2*4*(intensity-1) is the optimal size if we have two materials with different opacities
    Uint32 doX[8*MAX_BLOC_LIGHT], doY[8*MAX_BLOC_LIGHT], doXnext[8*MAX_BLOC_LIGHT], doYnext[8*MAX_BLOC_LIGHT];
    char intensity[8*MAX_BLOC_LIGHT], intensityNext[8*MAX_BLOC_LIGHT];
    doX[0] = x;
    doY[0] = y;
    intensity[0] = getBlock(terrain, x, y).light;
    int pointCount = 1;
    int nextPointCount = 0;

    int i;
    while(pointCount > 0)
    {
        for(i = 0; i < pointCount; i++)
        {
            char nextIntensity = intensity[i] - getBlockOpacity(terrain, doX[i], doY[i]);
            if(nextIntensity > 0)
            {
                if(getBlock(terrain, doX[i], doY[i]+1).light < nextIntensity)
                {
                    getBlockPtr(terrain, doX[i], doY[i]+1)->light = nextIntensity;
                    if(nextIntensity > 1)
                    {
                        doXnext[nextPointCount] = doX[i];
                        doYnext[nextPointCount] = doY[i]+1;
                        intensityNext[nextPointCount] = nextIntensity;
                        nextPointCount++;
                    }
                }
                if(getBlock(terrain, doX[i], doY[i]-1).light < nextIntensity)
                {
                    getBlockPtr(terrain, doX[i], doY[i]-1)->light = nextIntensity;
                    if(nextIntensity > 1)
                    {
                        doXnext[nextPointCount] = doX[i];
                        doYnext[nextPointCount] = doY[i]-1;
                        intensityNext[nextPointCount] = nextIntensity;
                        nextPointCount++;
                    }
                }
                if(getBlock(terrain, doX[i]+1, doY[i]).light < nextIntensity)
                {
                    getBlockPtr(terrain, doX[i]+1, doY[i])->light = nextIntensity;
                    if(nextIntensity > 1)
                    {
                        doXnext[nextPointCount] = doX[i]+1;
                        doYnext[nextPointCount] = doY[i];
                        intensityNext[nextPointCount] = nextIntensity;
                        nextPointCount++;
                    }
                }
                if(getBlock(terrain, doX[i]-1, doY[i]).light < nextIntensity)
                {
                    getBlockPtr(terrain, doX[i]-1, doY[i])->light = nextIntensity;
                    if(nextIntensity > 1)
                    {
                        doXnext[nextPointCount] = doX[i]-1;
                        doYnext[nextPointCount] = doY[i];
                        intensityNext[nextPointCount] = nextIntensity;
                        nextPointCount++;
                    }
                }
            }
        }
        memcpy(doX, doXnext, nextPointCount * sizeof(Uint32));
        memcpy(doY, doYnext, nextPointCount * sizeof(Uint32));
        memcpy(intensity, intensityNext, nextPointCount * sizeof(char));
        pointCount = nextPointCount;
        nextPointCount = 0;
    }
}

void breakBloc(Terrain* terrain, WaterManager* waterManager, Uint32 x, Uint32 y)
{
    if(checkBreakable(terrain->blocTypes, getBlock(terrain, x, y)))
    {
        if(getBlock(terrain, x, y).type == 2 || getBlock(terrain, x, y).type == 4) {
            switch(Random<int>::range(0, 2)) {
                case 0: soundstack_addSound(SOUND_TINK); break;
                case 1: soundstack_addSound(SOUND_TINK_ALT); break;
                case 2: soundstack_addSound(SOUND_TINK_ALT2); break;
            }
        }
        else
        {
            switch(Random<int>::range(0, 2)) {
                case 0: soundstack_addSound(SOUND_DIG); break;
                case 1: soundstack_addSound(SOUND_DIG_ALT); break;
                case 2: soundstack_addSound(SOUND_DIG_ALT2); break;
            }
        }

        if(getBlock(terrain, x, y).type == 6) deleteLightSource(terrain, x, y);

        #ifdef WATERMAN
        if(checkLiquid(terrain->blocTypes, getBlock(terrain, x-1, y))) waterman_addActiveBloc(waterManager, x-1, y);
        if(checkLiquid(terrain->blocTypes, getBlock(terrain, x+1, y))) waterman_addActiveBloc(waterManager, x+1, y);
        if(checkLiquid(terrain->blocTypes, getBlock(terrain, x, y-1))) waterman_addActiveBloc(waterManager, x, y-1);
        #endif // WATERMAN

        getBlockPtr(terrain, x, y)->type = 0;
    }
    else if(getBackwall(terrain, x, y).type != 0)
    {
        getBackwallPtr(terrain, x, y)->type = 0;
        switch(Random<int>::range(0, 2)) {
            case 0: soundstack_addSound(SOUND_DIG); break;
            case 1: soundstack_addSound(SOUND_DIG_ALT); break;
            case 2: soundstack_addSound(SOUND_DIG_ALT2); break;
        }
    }
    else {return;}

    processSunLightAroundBloc(terrain, x, y);

    SDL_Rect t = {int(x - MAX_BLOC_LIGHT), int(y - MAX_BLOC_LIGHT), 2*MAX_BLOC_LIGHT+1, 2*MAX_BLOC_LIGHT+1};
    clampWorld(terrain->width, terrain->height, &t, TERRAIN_BORDER);

	terrainRenderer->updateRect(t);  //Graphical update
}

void placeBloc(Terrain* terrain, Uint32 x, Uint32 y, Uint32 type)
{
    if(type < BACKWALL_TYPES_OFFSET) {if(getBlock(terrain, x, y).type != 0) return;}
    else {if(getBackwall(terrain, x, y).type != 0) return;}

    if(type == 6) addLightSource(terrain, x, y, SDL_Color{255, 255, 255, 0}, MAX_BLOC_LIGHT, 8, 8);

    if(type < BACKWALL_TYPES_OFFSET)
        getBlockPtr(terrain, x, y)->type = type;
    else
        getBackwallPtr(terrain, x, y)->type = type - BACKWALL_TYPES_OFFSET;

    processSunLightAroundBloc(terrain, x, y);

    SDL_Rect t = {int(x - MAX_BLOC_LIGHT), int(y - MAX_BLOC_LIGHT), 2*MAX_BLOC_LIGHT+1, 2*MAX_BLOC_LIGHT+1};
    clampWorld(terrain->width, terrain->height, &t, TERRAIN_BORDER);

	terrainRenderer->updateRect(t);

    switch(Random<int>::range(0, 2)) {
        case 0: soundstack_addSound(SOUND_DIG); break;
        case 1: soundstack_addSound(SOUND_DIG_ALT); break;
        case 2: soundstack_addSound(SOUND_DIG_ALT2); break;
    }
}

