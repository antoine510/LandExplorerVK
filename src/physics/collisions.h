#pragma once

#include "../common.h"
#include "../terrain/terrain.h"
#include "../entities.h"
#include "sweepAndPrune.h"

#define COLLTYPE_PLAYER     1
#define COLLTYPE_PASSIVE    2
#define COLLTYPE_MELEE      3
#define COLLTYPE_WEAPON     4
#define COLLTYPE_TRIGGER    5
#define COLLTYPE_NOCOLL     255

#define COLL_HIT_DELAY 0.5f

void checkTerrainCols(Terrain* terrain, Entities* entities);
void processEntityColls(SAP* sap, Entities* entities);

int entityInLiquid(CollisionData* collData, Terrain* terrain);
Vec2 processHitDirection(Entities* entities, int masterID, int slaveID);
float processDistance(Entities* entities, int id1, int id2);
float processXDistance(Entities* entities, int id1, int id2);
float processYDistance(Entities* entities, int id1, int id2);
