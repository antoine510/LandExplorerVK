#pragma once

#include "../common.h"
#include "../terrain/terrain.h"
#include "../entities.h"

#define ITEM_COUNT 1024

typedef struct Item {
    int id;
    int maxAmount;
    int usageTime;
} Item;

void useItem(Item* item, Level* level, Uint32 x, Uint32 y);

