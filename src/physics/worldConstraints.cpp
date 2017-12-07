#include "worldConstraints.h"
#include "terrain/blockTypes.h"
#include "collisions.h"

void applyConstraints(Terrain* terrain, Entities* entities)
{
    Uint32 target = COMP_COLLIDEABLE + COMP_DYNAMIC + COMP_RECT;
    for(int i = 0; i < ENTITY_COUNT; i++)
    {
        if((entities->components[i] & target) == target)
        {
            Vec2* velocity = &entities->dynData[i].velocity;
            if(entityInLiquid(&entities->collData[i], terrain))
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
