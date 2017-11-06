#include "item.h"
#include "../terrain/blockTypes.h"
#include "../terrain/terrainGeneration.h"
#include "../entityModels.h"
#include "../level.h"
#include "../sound.h"

#define SWEEP_TIME 30

static void useSword(Item* item, Level* level);

void useItem(Item* item, Level* level, Uint32 x, Uint32 y)
{
    if(item->id >= 0 && item->id < 2*BACKWALL_TYPES_OFFSET)
    {
        placeBloc(level->terrain, x, y, item->id);
    }
    else
    {
        switch(item->id - 2*BACKWALL_TYPES_OFFSET)
        {
        case 0:
            useSword(item, level);
            break;
        case 1:
            break;
        default: break;
        }
    }
}

void useSword(Item* item, Level* level)
{
    if(level->playerControl->currentWeaponID != -1) return;
    int id = getFreeEntity(level->entities);
    setEntityAsModel(level->entities, id, "sword");
    level->entities->weaponData[id].parentID = level->playerID;
    level->playerControl->currentWeaponID = id;
    soundstack_addSound(SOUND_SWING_SWORD);
}

