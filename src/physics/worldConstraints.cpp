#include "worldConstraints.h"
#include "terrain/blockTypes.h"
#include "collisions.h"

void applyConstraints(Terrain* terrain, Entities* entities)
{
    int i;
    Uint32 target = COMP_COLLIDEABLE + COMP_DYNAMIC + COMP_RECT;
    for(i = 0; i < ENTITY_COUNT; i++)
    {
        if((entities->components[i] & target) == target)
        {
            Vec2* velocity = &entities->dynData[i].velocity;
            if(entityInBlock(&entities->collData[i], terrain, BT_LIQUID))
            {
                if(velocity->y < MAX_DOWNWARD_VELOCITY / WATER_DAMPENING) velocity->y += GRAVITY / PCPS / WATER_DAMPENING;
                velocity->x *= 1.0f - WATER_DAMPENING / PCPS;
                velocity->y *= 1.0f - WATER_DAMPENING / PCPS;
            }
            else
            {
                if(velocity->y < MAX_DOWNWARD_VELOCITY) velocity->y += GRAVITY / PCPS;
            }
        }
    }
    processEntityColls(entities->sap, entities);
    checkTerrainCols(terrain, entities);
}
