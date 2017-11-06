#pragma once

#include "../common.h"
#include "item.h"

#define INVENTORY_WIDTH 8
#define INVENTORY_HEIGHT 4

typedef struct Inventory {
    Item items[ITEM_COUNT];
    int itemID[INVENTORY_WIDTH * INVENTORY_HEIGHT];
    int itemCount[INVENTORY_WIDTH * INVENTORY_HEIGHT];
    int selection;
    bool active;
} Inventory;

Inventory* initInventory();

void exchangeItem(Inventory* inventory, int* itemID, int* count, int index);
bool addItemInFreeSlot(Inventory* inventory, int itemID);

void useCurrentItem(Inventory* inventory, Level* level, Uint32 x, Uint32 y);
void updateSelection(Inventory* inventory);

int getSlotIndex(int x, int y);
Item getCurrentItem(Inventory* inventory);

void destroyInventory(Inventory* inventory);

