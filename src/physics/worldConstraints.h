#pragma once

#include "../common.h"
#include "../terrain/terrain.h"
#include "../entities.h"

#define GRAVITY 40.0f                           // Blocks per second per second

#define MAX_DOWNWARD_VELOCITY 25.0f              // Blocks per second
#define WATER_DAMPENING 2.0f
#define GROUND_BREAKING_VELOCITY (30.0f)

void applyConstraints(Terrain* terrain, Entities* entities);

