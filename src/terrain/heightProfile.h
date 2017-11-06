#pragma once

#include "../common.h"
#include "terrain.h"

#define TERRAINGEN_UNDERGROUND_LIMIT 0.5f
#define OCEAN_HEIGHT 0.45f

#define OCEAN_LOW_SCALE 0.0f
#define OCEAN_MEDIUM_SCALE 0.0f
#define OCEAN_HIGH_SCALE 0.0f

#define OCEAN_TRANSITION_LENGTH 50
#define OCEAN_LIMIT 66

#define PLAINS_LOW_SCALE 10.0f
#define PLAINS_MEDIUM_SCALE 15.0f
#define PLAINS_HIGH_SCALE 15.0f

#define MOUNTAINS_LOW_SCALE 20.0f
#define MOUNTAINS_MEDIUM_SCALE 60.0f
#define MOUNTAINS_HIGH_SCALE 150.0f

#define PLAINS_TRANSITION_LENGTH (terrain->width/8)
#define PLAINS_LIMIT (terrain->width/2)

#define MOUNTAINS_TRANSITION_LENGTH (terrain->width/16)
#define MOUNTAINS_LIMIT (terrain->width - MOUNTAINS_TRANSITION_LENGTH - OCEAN_LIMIT)

typedef struct HeightProfile {
    float* lowScale;
    float* mediumScale;
    float* highScale;
} HeightProfile;

void generateHeightProfile(HeightProfile* heightProfile, Terrain* terrain);
void destroyHeightProfile(HeightProfile* hProfile);

