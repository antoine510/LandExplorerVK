#include "heightProfile.h"
#include "terrainGeneration.h"

void generateHeightProfile(HeightProfile* heightProfile, Terrain* terrain)
{
    heightProfile->lowScale = (float*)calloc(terrain->width, sizeof(float));
    heightProfile->mediumScale = (float*)calloc(terrain->width, sizeof(float));
    heightProfile->highScale = (float*)calloc(terrain->width, sizeof(float));
    int x;
    for(x = 0; x < OCEAN_LIMIT; x++)
    {
        heightProfile->lowScale[x] = OCEAN_LOW_SCALE;
        heightProfile->mediumScale[x] = OCEAN_MEDIUM_SCALE;
        heightProfile->highScale[x] = OCEAN_HIGH_SCALE;
    }
    for(x = OCEAN_LIMIT; x < OCEAN_LIMIT + OCEAN_TRANSITION_LENGTH; x++)
    {
        float coef = (float)(x - OCEAN_LIMIT) / OCEAN_TRANSITION_LENGTH;
        heightProfile->lowScale[x] =    (1.0f-coef)*OCEAN_LOW_SCALE +      coef*PLAINS_LOW_SCALE;
        heightProfile->mediumScale[x] = (1.0f-coef)*OCEAN_MEDIUM_SCALE +   coef*PLAINS_MEDIUM_SCALE;
        heightProfile->highScale[x] =   (1.0f-coef)*OCEAN_HIGH_SCALE +     coef*PLAINS_HIGH_SCALE;
    }
    for(x = OCEAN_LIMIT + OCEAN_TRANSITION_LENGTH; x < PLAINS_LIMIT; x++)
    {
        heightProfile->lowScale[x] = PLAINS_LOW_SCALE;
        heightProfile->mediumScale[x] = PLAINS_MEDIUM_SCALE;
        heightProfile->highScale[x] = PLAINS_HIGH_SCALE;
    }
    for(x = PLAINS_LIMIT; x < PLAINS_LIMIT + PLAINS_TRANSITION_LENGTH; x++)
    {
        float coef = (float)(x - PLAINS_LIMIT) / PLAINS_TRANSITION_LENGTH;
        heightProfile->lowScale[x] =    (1.0f-coef)*PLAINS_LOW_SCALE +      coef*MOUNTAINS_LOW_SCALE;
        heightProfile->mediumScale[x] = (1.0f-coef)*PLAINS_MEDIUM_SCALE +   coef*MOUNTAINS_MEDIUM_SCALE;
        heightProfile->highScale[x] =   (1.0f-coef)*PLAINS_HIGH_SCALE +     coef*MOUNTAINS_HIGH_SCALE;
    }
    for(x = PLAINS_LIMIT + PLAINS_TRANSITION_LENGTH; x < MOUNTAINS_LIMIT; x++)
    {
        heightProfile->lowScale[x] = MOUNTAINS_LOW_SCALE;
        heightProfile->mediumScale[x] = MOUNTAINS_MEDIUM_SCALE;
        heightProfile->highScale[x] = MOUNTAINS_HIGH_SCALE;
    }
    for(x = MOUNTAINS_LIMIT; x < MOUNTAINS_LIMIT + MOUNTAINS_TRANSITION_LENGTH; x++)
    {
        float coef = (float)(x - MOUNTAINS_LIMIT) / MOUNTAINS_TRANSITION_LENGTH;
        heightProfile->lowScale[x] =    (1.0f-coef)*MOUNTAINS_LOW_SCALE +      coef*OCEAN_LOW_SCALE;
        heightProfile->mediumScale[x] = (1.0f-coef)*MOUNTAINS_MEDIUM_SCALE +   coef*OCEAN_MEDIUM_SCALE;
        heightProfile->highScale[x] =   (1.0f-coef)*MOUNTAINS_HIGH_SCALE +     coef*OCEAN_HIGH_SCALE;
    }
    for(x = MOUNTAINS_LIMIT + MOUNTAINS_TRANSITION_LENGTH; x < terrain->width; x++)
    {
        heightProfile->lowScale[x] = OCEAN_LOW_SCALE;
        heightProfile->mediumScale[x] = OCEAN_MEDIUM_SCALE;
        heightProfile->highScale[x] = OCEAN_HIGH_SCALE;
    }
}

void destroyHeightProfile(HeightProfile* hProfile)
{
    free(hProfile->lowScale);
    free(hProfile->mediumScale);
    free(hProfile->highScale);
}

