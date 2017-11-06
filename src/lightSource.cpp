#include "utility/mathUtility.h"
#include "lightSource.h"
#include "terrain/terrain.h"
#include <algorithm>


void updateLightSource(LightSource* light, Terrain* terrain)
{
    memset(light->mask, 0, LIGHT_MASK_SIZE*LIGHT_MASK_SIZE);

    //2*4*(intensity-1) is the optimal size if we have two materials with different opacities
	const unsigned int blocCount = 8 * (light->intensity - 1);
	Uint32* doX = (Uint32*)malloc(2 * blocCount * sizeof(Uint32));	//Allow space for current and next set of data
	Uint32* doY = (Uint32*)malloc(2 * blocCount * sizeof(Uint32));
	char* intensity = (char*)malloc(2 * blocCount);

	Uint32* doXnext = doX + blocCount;
	Uint32* doYnext = doY + blocCount;
	char* intensityNext = intensity + blocCount;

    doX[0] = LIGHT_MASK_SIZE/2;
    doY[0] = LIGHT_MASK_SIZE/2;
    intensity[0] = light->intensity;
    light->mask[getMaskIndex(doX[0], doY[0])] = light->intensity;
    int pointCount = 1;
    int nextPointCount = 0;

    int i;
    while(pointCount > 0)
    {
        for(i = 0; i < pointCount; i++)
        {
            char nextIntensity = intensity[i] - std::max((Uint8)1,getBlockOpacity(terrain, doX[i] + light->x - LIGHT_MASK_SIZE/2, doY[i] + light->y - LIGHT_MASK_SIZE/2));
            if(nextIntensity > 0)
            {
                if(light->mask[getMaskIndex(doX[i], doY[i]+1)] < nextIntensity)
                {
                    light->mask[getMaskIndex(doX[i], doY[i]+1)] = nextIntensity;
                    if(nextIntensity > 1 && doY[i] < LIGHT_MASK_SIZE-2)
                    {
                        doXnext[nextPointCount] = doX[i];
                        doYnext[nextPointCount] = doY[i]+1;
                        intensityNext[nextPointCount] = nextIntensity;
                        nextPointCount++;
                    }
                }
                if(light->mask[getMaskIndex(doX[i], doY[i]-1)] < nextIntensity)
                {
                    light->mask[getMaskIndex(doX[i], doY[i]-1)] = nextIntensity;
                    if(nextIntensity > 1 && doY[i] > 1)
                    {
                        doXnext[nextPointCount] = doX[i];
                        doYnext[nextPointCount] = doY[i]-1;
                        intensityNext[nextPointCount] = nextIntensity;
                        nextPointCount++;
                    }
                }
                if(light->mask[getMaskIndex(doX[i]+1, doY[i])] < nextIntensity)
                {
                    light->mask[getMaskIndex(doX[i]+1, doY[i])] = nextIntensity;
                    if(nextIntensity > 1 && doX[i] < LIGHT_MASK_SIZE-2)
                    {
                        doXnext[nextPointCount] = doX[i]+1;
                        doYnext[nextPointCount] = doY[i];
                        intensityNext[nextPointCount] = nextIntensity;
                        nextPointCount++;
                    }
                }
                if(light->mask[getMaskIndex(doX[i]-1, doY[i])] < nextIntensity)
                {
                    light->mask[getMaskIndex(doX[i]-1, doY[i])] = nextIntensity;
                    if(nextIntensity > 1 && doX[i] > 1)
                    {
                        doXnext[nextPointCount] = doX[i]-1;
                        doYnext[nextPointCount] = doY[i];
                        intensityNext[nextPointCount] = nextIntensity;
                        nextPointCount++;
                    }
                }
            }
        }

		Uint32* t = doX; doX = doXnext; doXnext = t;
		t = doY; doY = doYnext; doYnext = t;
		char* tc = intensity; intensity = intensityNext; intensityNext = tc;

        pointCount = nextPointCount;
        nextPointCount = 0;
    }

	free(doX);
	free(doY);
	free(intensity);
}

Uint16 getMaskIndex(Uint8 x, Uint8 y)
{
    return x * LIGHT_MASK_SIZE + y;
}

void updateLightSources(Terrain* terrain, Uint32 x, Uint32 y)
{
    int i;
    Chunck* chunck = &terrain->chuncks[getBlocChunck(x, y)];
    for(i = 0; i < chunck->lightCount; i++)
    {
        LightSource* light = &chunck->lights[i];
        if(getDistance(light->x, light->y, x, y) < MAX_BLOC_LIGHT)
        {
            updateLightSource(light, terrain);
        }
    }
}

SDL_Color getLight(LightSource* light, Uint32 x, Uint32 y)
{
    Uint8 intensity = light->mask[getMaskIndex(x - light->x + LIGHT_MASK_SIZE/2, y - light->y + LIGHT_MASK_SIZE/2)];
    return SDL_Color{(light->color.r * (intensity+1u)) >> 4u,
                       (light->color.g * (intensity+1u)) >> 4u,
                       (light->color.b * (intensity+1u)) >> 4u, intensity};      //The alpha component is the intensity
}

void getLightSourcePosition(LightSource* light, Uint32* x, Uint32* y)
{
    *x = light->x * BLOC_SIZE + light->xOffset;
    *y = light->y * BLOC_SIZE + light->yOffset;
}

void addLightSource(Terrain* terrain, Uint32 x, Uint32 y, SDL_Color color, Uint8 intensity, int xOffset, int yOffset)
{
    Chunck* chunck = &terrain->chuncks[getBlocChunck(x, y)];
    int lightIndex = chunck->lightCount++;
    LightSource* lightSources = chunck->lights;

    //We reallocate the array to the new size
    lightSources = (LightSource*)realloc(lightSources, (lightIndex+1) * sizeof(LightSource));

    lightSources[lightIndex].x = x;
    lightSources[lightIndex].y = y;
    lightSources[lightIndex].color = color;
    lightSources[lightIndex].intensity = intensity;
    lightSources[lightIndex].xOffset = xOffset;
    lightSources[lightIndex].yOffset = yOffset;

    chunck->lights = lightSources;
}

void addTorch(Terrain* terrain, Uint32 x, Uint32 y)
{
    SDL_Color color = {255, 255, 255, 255};
    addLightSource(terrain, x, y, color, MAX_BLOC_LIGHT, 8, 8);
}

void deleteLightSource(Terrain* terrain, Uint32 x, Uint32 y)
{
    Chunck* chunck = &terrain->chuncks[getBlocChunck(x, y)];

    LightSource* lightSource = chunck->lights;
    int i, lightCount = --chunck->lightCount, lightIndex = lightCount;
    for(i = 0; i < lightCount; i++)
    {
        if(lightSource[i].x == x && lightSource[i].y == y) {lightIndex = i; break;}
    }
    //The objects cannot overlap, memcpy behaviour would be undefined
    if(lightIndex != lightCount)
        memcpy(lightSource + lightIndex, lightSource + lightCount, sizeof(LightSource));
}

