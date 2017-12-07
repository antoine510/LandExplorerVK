#include "collisions.h"
#include "terrain/blockTypes.h"
#include "worldConstraints.h"
#include "utility/mathUtility.h"
#include "utility/random.h"
#include "timer.h"
#include "sound.h"

static void processHit(Entities* entities, int id, int damage, float knockback, Vec2 direction);
static void groundSlowing(Entities* entities, int id);
static void oneBlocHop(Terrain* terrain, Entities* entities, unsigned int id, char hCol, Vec2 v);

static void processTerrainCols(Terrain* terrain, Entities* entities, unsigned int id, char vCol, char hCol, Vec2 v);

void checkTerrainCols(Terrain* terrain, Entities* entities)
{
    int i;
    Uint32 target = COMP_DYNAMIC + COMP_COLLIDEABLE + COMP_RECT;
    for(i = 0; i < ENTITY_COUNT; i++)
    {
        if((entities->components[i] & target) == target)
        {
            Vec2* pos = &entities->collData[i].pos;
            float w = entities->collData[i].sz.x;
            float h = entities->collData[i].sz.y;

            float vx = entities->dynData[i].velocity.x / PCPS;
            float vy = entities->dynData[i].velocity.y / PCPS;

            int x, y;
            int hCollision = 0, vCollision = 0;
            entities->dynData[i].touchGround = 0;

            //Left collisions
            for(x = (int)pos->x -1; x >= (int)(pos->x + vx) && !hCollision; x--)
            {
                for(y = (int)pos->y; y <= exclusive(pos->y + h) && !hCollision; y++)
                {
                    if(checkSolid(terrain->blocTypes, getBlock(terrain, x, y))){
                        hCollision = -1;
                        pos->x = (float)x + 1.0f;                                        //set a valid position
                        entities->dynData[i].velocity.x = 0;
                    }
                }
            }

            //Right collisions
            for(x = exclusive(pos->x + w)+1; x <= exclusive(pos->x + w + vx) && !hCollision; x++)
            {
                for(y = (int)pos->y; y <= exclusive(pos->y + h) && !hCollision; y++)
                {
                    if(checkSolid(terrain->blocTypes, getBlock(terrain, x, y))){
                        hCollision = 1;
                        pos->x = (float)x - w;
                        entities->dynData[i].velocity.x = 0;
                    }
                }
            }

            if(hCollision == 0)
            {
                pos->x += vx;
            }

            //Up collisions
            for(y = (int)pos->y -1; y >= (int)(pos->y + vy) && !vCollision; y--)
            {
                for(x = (int)pos->x; x <= exclusive(pos->x + w) && !vCollision; x++)
                {
                    if(checkSolid(terrain->blocTypes, getBlock(terrain, x, y))){
                        vCollision = 1;
                        pos->y = (float)y + 1.0f;
                        entities->dynData[i].velocity.y = 0;
                    }
                }
            }

            //Down collisions
            for(y = exclusive(pos->y + h)+1; y <= exclusive(pos->y + h + vy) && !vCollision; y++)
            {
                for(x = (int)pos->x; x <= exclusive(pos->x + w) && !vCollision; x++)
                {
                    if(checkSolid(terrain->blocTypes, getBlock(terrain, x, y))){
                        vCollision = 1;
                        entities->dynData[i].touchGround = 1;
                        pos->y = (float)y - h;
                        entities->dynData[i].velocity.y = 0;
                    }
                }
            }

            if(!vCollision)
            {
                pos->y += vy;
            }

            if(vCollision || hCollision) processTerrainCols(terrain, entities, i, vCollision, hCollision, Vec2{vx, vy});

            entities->rect[i].x = (int)(pos->x * BLOC_SIZE);     //Graphical translation
            entities->rect[i].y = (int)(pos->y * BLOC_SIZE);
        }
    }
}

int entityInLiquid(CollisionData* collData, Terrain* terrain)
{
    int x, y;
    for (x = (int)collData->pos.x; x <= exclusive(collData->pos.x + collData->sz.x); x++)
    {
        for (y = (int)collData->pos.y; y <= exclusive(collData->pos.y + collData->sz.y); y++)
        {
            if(checkLiquid(terrain->blocTypes, getBlock(terrain, x, y))) return 1;
        }
    }
    return 0;
}

void processHit(Entities* entities, int id, int damage, float knockback, Vec2 direction)
{
    if(entities->collData[id].collTime > COLL_HIT_DELAY * PCPS)
    {
        entities->status[id].HP -= damage;
		direction *= knockback;
		entities->dynData[id].velocity += direction;

        if(entities->status[id].HP > 0)
        {
            if(entities->collData[id].collType == COLLTYPE_PLAYER)
            {
                switch(Random<int>::range(0, 2)) {
                    case 0: soundstack_addSound(SOUND_PLAYER_HIT); break;
                    case 1: soundstack_addSound(SOUND_PLAYER_HIT_ALT); break;
                    case 2: soundstack_addSound(SOUND_PLAYER_HIT_ALT2); break;
                }
            }
            else if(entities->collData[id].collType == COLLTYPE_PASSIVE)
            {
                soundstack_addSound(SOUND_HIT_PASSIVE);
            }
            else if(entities->collData[id].collType == COLLTYPE_MELEE)
            {
                soundstack_addSound(SOUND_HIT_ENEMY);
            }
        }
        else
        {
            switch(entities->collData[id].collType)
            {
            case COLLTYPE_PLAYER:
                soundstack_addSound(SOUND_PLAYER_DEATH);
                entities->collData[id].collType = COLLTYPE_NOCOLL;
                break;
            case COLLTYPE_PASSIVE:
                destroyEntity(entities, id);
                soundstack_addSound(SOUND_KILL_PASSIVE);
                break;
            case COLLTYPE_MELEE:
                destroyEntity(entities, id);
                soundstack_addSound(SOUND_KILL_ENEMY);
                break;
            }
        }
        entities->collData[id].collTime = 0;
    }
}

void processEntityColls(SAP* sap, Entities* entities)
{
    int i;
    for(i = 0; i < sap->candidatesCount; i++)
    {
        if(sap->candidates[i].yValid)
        {
            int entID1 = sap->candidates[i].c1.entityID, entID2 = sap->candidates[i].c2.entityID;
            int masterID, slaveID;
            if(entities->collData[entID1].collType < entities->collData[entID2].collType)
                {masterID = entID1; slaveID = entID2;}
            else
                {masterID = entID2; slaveID = entID1;}

            switch(entities->collData[masterID].collType)
            {
            case COLLTYPE_PLAYER:
                switch(entities->collData[slaveID].collType)
                {
                case COLLTYPE_PLAYER:
                    /* No multiplayer yet :( */
                    break;
                case COLLTYPE_PASSIVE:
                    break;
                case COLLTYPE_MELEE:
                    processHit(entities, masterID, 10, entities->status[slaveID].knockback, processHitDirection(entities, masterID, slaveID));
                    break;
                case COLLTYPE_WEAPON:
                    if(entities->weaponData[slaveID].parentID != masterID)
                    {
                        switch(entities->weaponData[slaveID].type)
                        {
                        case weapon_sword:
                            processHit(entities, masterID, entities->weaponData[slaveID].damage,
                                    entities->weaponData[slaveID].knockback,
                                    processHitDirection(entities, masterID, entities->weaponData[slaveID].parentID));
                            break;
                        case weapon_arrow:
                            processHit(entities, masterID, entities->weaponData[slaveID].damage,
                                   entities->weaponData[slaveID].knockback, glm::normalize(entities->dynData[slaveID].velocity));
                            destroyEntity(entities, slaveID);
                            break;
                        }
                    }
                    break;
                case COLLTYPE_TRIGGER:
                    entities->triggerData[slaveID].trigger(entities->triggerData[slaveID].data);
                    break;
                }
                break;
            case COLLTYPE_PASSIVE:
                switch(entities->collData[slaveID].collType)
                {
                case COLLTYPE_PASSIVE:
                    break;
                case COLLTYPE_MELEE:
                    processHit(entities, masterID, 10, entities->status[slaveID].knockback, processHitDirection(entities, masterID, slaveID));
                    break;
                case COLLTYPE_WEAPON:
                    if(entities->weaponData[slaveID].parentID != masterID)
                    {
                        switch(entities->weaponData[slaveID].type)
                        {
                        case weapon_sword:
                            processHit(entities, masterID, entities->weaponData[slaveID].damage,
                                    entities->weaponData[slaveID].knockback,
                                    processHitDirection(entities, masterID, entities->weaponData[slaveID].parentID));
                            break;
                        case weapon_arrow:
                            processHit(entities, masterID, entities->weaponData[slaveID].damage,
                                   entities->weaponData[slaveID].knockback, processHitDirection(entities, masterID, slaveID));
                            destroyEntity(entities, slaveID);
                            break;
                        }
                    }
                    break;
                }
                break;
            case COLLTYPE_MELEE:
                switch(entities->collData[slaveID].collType)
                {
                case COLLTYPE_MELEE:
                    if(entities->aiData[masterID].targetID == slaveID)
                    {
                        processHit(entities, slaveID, 10, entities->status[masterID].knockback, processHitDirection(entities, slaveID, masterID));
                        entities->aiData[slaveID].targetID = masterID;
                    }
                    if(entities->aiData[slaveID].targetID == masterID)
                    {
                        processHit(entities, masterID, 10, entities->status[slaveID].knockback, processHitDirection(entities, masterID, slaveID));
                        entities->aiData[masterID].targetID = slaveID;
                    }
                    break;
                case COLLTYPE_WEAPON:
                    if(entities->weaponData[slaveID].parentID != masterID)
                    {
                        switch(entities->weaponData[slaveID].type)
                        {
                        case weapon_sword:
                            processHit(entities, masterID, entities->weaponData[slaveID].damage,
                                    entities->weaponData[slaveID].knockback,
                                    processHitDirection(entities, masterID, entities->weaponData[slaveID].parentID));
                            break;
                        case weapon_arrow:
                            processHit(entities, masterID, entities->weaponData[slaveID].damage,
                                   entities->weaponData[slaveID].knockback, processHitDirection(entities, masterID, slaveID));

                            //Let the fight begin
                            entities->aiData[masterID].targetID = entities->weaponData[slaveID].parentID;

                            destroyEntity(entities, slaveID);
                            break;
                        }
                    }
                    break;
                }
                break;
            default: break;
            }
        }
    }
}

Vec2 processHitDirection(Entities* entities, int masterID, int slaveID)
{
    Vec2 hitDirection = {entities->collData[masterID].pos.x + entities->collData[masterID].sz.x/2 -
                         (entities->collData[slaveID].pos.x + entities->collData[slaveID].sz.x/2),
                          entities->collData[masterID].pos.y + entities->collData[masterID].sz.y/2 -
                         (entities->collData[slaveID].pos.y + entities->collData[slaveID].sz.y/2)};
    return glm::normalize(hitDirection);
}

float processXDistance(Entities* entities, int id1, int id2)
{
    return fabsf(entities->collData[id1].pos.x + entities->collData[id1].sz.x/2 - (entities->collData[id2].pos.x + entities->collData[id2].sz.x/2));
}

float processYDistance(Entities* entities, int id1, int id2)
{
    return fabsf(entities->collData[id1].pos.y + entities->collData[id1].sz.y/2 - (entities->collData[id2].pos.y + entities->collData[id2].sz.y/2));
}

float processDistance(Entities* entities, int id1, int id2)
{
    return glm::length(Vec2(processXDistance(entities, id1, id2), processYDistance(entities, id1, id2)));
}

//The entity is Dynamic and Collideable
void processTerrainCols(Terrain* terrain, Entities* entities, unsigned int id, char vCol, char hCol, Vec2 v)
{
    switch(entities->collData[id].collType)
    {
    case COLLTYPE_PLAYER:
        oneBlocHop(terrain, entities, id, hCol, v);
        groundSlowing(entities, id);
        break;
    case COLLTYPE_PASSIVE:
        groundSlowing(entities, id);
        break;
    case COLLTYPE_MELEE:
        oneBlocHop(terrain, entities, id, hCol, v);
        groundSlowing(entities, id);
        break;
    case COLLTYPE_WEAPON:
        switch(entities->weaponData[id].type)
        {
        case weapon_sword: break;
        case weapon_arrow:
            destroyEntity(entities, id);
            soundstack_addSound(SOUND_DIG);
            break;
        }
        break;
    case COLLTYPE_NOCOLL:
        groundSlowing(entities, id);
        break;
    }
}

void groundSlowing(Entities* entities, int id)
{
    //Slow down while touching the ground
    if(entities->dynData[id].touchGround && !entities->dynData[id].moving)
    {
        float* velx = &entities->dynData[id].velocity.x;
        if(*velx > GROUND_BREAKING_VELOCITY / PCPS) {*velx -= GROUND_BREAKING_VELOCITY / PCPS;}
        else if(*velx < -GROUND_BREAKING_VELOCITY / PCPS) {*velx += GROUND_BREAKING_VELOCITY / PCPS;}
        else {*velx = 0;}
    }
}

void oneBlocHop(Terrain* terrain, Entities* entities, unsigned int id, char hCol, Vec2 v)
{
    Vec2* pos = &entities->collData[id].pos;
    float w = entities->collData[id].sz.x;
    float h = entities->collData[id].sz.y;

    //If there is only one block at his feet the entity can go on
    if(hCol && entities->dynData[id].touchGround)
    {
        bool invalid = false;
        int x, y;
        for(x = (int)pos->x; x <= exclusive(pos->x + w)  && !invalid; x++)
        {
            invalid |= checkSolid(terrain->blocTypes, getBlock(terrain, x, (int)pos->y -1));
        }

        if(hCol == 1 && checkSolid(terrain->blocTypes, getBlock(terrain, exclusive(pos->x + w)+1, exclusive(pos->y + h))))
        {
            for(y = (int)pos->y; y <= exclusive(pos->y + h) && !invalid; y++)
            {
                invalid |= checkSolid(terrain->blocTypes, getBlock(terrain, exclusive(pos->x + w)+1, y-1));
            }
        }
        else if(hCol == -1 && checkSolid(terrain->blocTypes, getBlock(terrain, (int)pos->x -1, exclusive(pos->y + h))))
        {
            for(y = (int)pos->y; y <= exclusive(pos->y + h) && !invalid; y++)
            {
                invalid |= checkSolid(terrain->blocTypes, getBlock(terrain, (int)pos->x -1, y-1));
            }
        }
        else {invalid = true;}
        if(!invalid)
        {
            pos->y--;
            entities->dynData[id].velocity.x = v.x * PCPS;
        }
    }
}

