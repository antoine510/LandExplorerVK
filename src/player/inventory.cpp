#include "inventory.h"
#include "../keyStates.h"
#include "../sound.h"
#include <algorithm>

Inventory* initInventory()
{
    Inventory* inventory = (Inventory*)calloc(1, sizeof(Inventory));
    inventory->selection = 0;
    int i;
    for(i = 0; i < ITEM_COUNT; i++) {inventory->items[i].id = i; inventory->items[i].usageTime = 50;}
    inventory->itemID[0] = 1; inventory->itemCount[0] = 1;
    inventory->itemID[1] = 2*BACKWALL_TYPES_OFFSET +0; inventory->itemCount[1] = 1;
    inventory->itemID[2] = 2; inventory->itemCount[2] = 1;
    inventory->itemID[3] = 258; inventory->itemCount[3] = 1;
    inventory->itemID[4] = 3; inventory->itemCount[4] = 1;
    inventory->itemID[5] = 4; inventory->itemCount[5] = 1;
    inventory->itemID[6] = 5; inventory->itemCount[6] = 1;
    inventory->itemID[7] = 6; inventory->itemCount[7] = 1;

    return inventory;
}

void exchangeItem(Inventory* inventory, int* itemID, int* count, int index)
{
    int oldItemID = inventory->itemID[index], oldItemCount = inventory->itemCount[index];
    if(oldItemID != *itemID)
    {
        inventory->itemID[index] = *itemID;
        inventory->itemCount[index] = *count;
        *itemID = oldItemID;
        *count = oldItemCount;
    }
    else
    {
        int finalCount = std::min(inventory->itemCount[index] + *count, inventory->items[oldItemID].maxAmount);
        inventory->itemCount[index] = finalCount;
        *count = inventory->itemCount[index] + *count - finalCount;
    }
    soundstack_addSound(SOUND_GRAB);
}

bool addItemInFreeSlot(Inventory* inventory, int itemID)
{
    int i;
    for(i = 0; i < INVENTORY_WIDTH * INVENTORY_HEIGHT; i++)
    {
        if(inventory->itemCount[i] != 0 && inventory->itemID[i] == itemID)
        {
            if(inventory->itemCount[i] < inventory->items[itemID].maxAmount) {inventory->itemCount[i]++; return true;}
        }
    }
    for(i = 0; i < INVENTORY_WIDTH * INVENTORY_HEIGHT; i++)
    {
        if(inventory->itemCount[i] == 0) {inventory->itemID[i] = itemID; return true;}
    }
    return false;
}

void useCurrentItem(Inventory* inventory, Level* level, Uint32 x, Uint32 y)
{
    if(inventory->itemCount[inventory->selection] > 0)
        useItem(&inventory->items[inventory->itemID[inventory->selection]], level, x, y);
}

void updateSelection(Inventory* inventory)
{
    int old = inventory->selection;

    mouseWheelIncrement(&inventory->selection);
    std::clamp(inventory->selection, 0, 7);

    if(inventory->selection != old) soundstack_addSound(SOUND_MENU_TICK);
}

Item getCurrentItem(Inventory* inventory)
{
    return inventory->items[inventory->selection];
}

int getSlotIndex(int x, int y) {return y*INVENTORY_WIDTH + x;}

void destroyInventory(Inventory* inventory)
{
    free(inventory);
}
