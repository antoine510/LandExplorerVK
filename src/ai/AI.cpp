#include "AI.h"
#include "entityModels.h"
#include "physics/collisions.h"
#include "sound.h"
#include "utility/random.h"
#include <algorithm>

#define AI_ARCHER_RELOAD_TIME 3.0f

static void ai_updateSheep(Entities* entities, int index);
static void ai_updateArcher(Entities* entities, int index);
static void ai_updateGuard(Entities* entities, int index);

void ai_update(Entities* entities, Terrain* terrain) {
	int i;
	Uint32 target = COMP_AI;
	for(i = 0; i < ENTITY_COUNT; i++) {
		if((entities->components[i] & target) == target) {
			switch(entities->aiData[i].aiType) {
			case ai_sheep:
				ai_updateSheep(entities, i);
				break;
			case ai_archer:
				ai_updateArcher(entities, i);
				break;
			case ai_guard:
				ai_updateGuard(entities, i);
				break;
			}
		}
	}
}

void ai_updateSheep(Entities* entities, int index) {
	entities->aiData[index].duration -= 1.0f / AISPS;
	if(entities->aiData[index].duration < 0) {
		if(entities->dynData[index].direction)
			entities->dynData[index].direction = 0;
		else {
			entities->dynData[index].direction = Random<int>::range(0, 1) * 2 - 1;
			entities->gfxData[index].texState = (entities->dynData[index].direction == 1) ? 0 : 1;
		}
		entities->aiData[index].duration = Random<float>::range(5.0f, 12.0f);
	} else if(entities->dynData[index].direction != 0 && !entities->dynData[index].velocity.x && entities->dynData[index].touchGround) {
		entities->dynData[index].velocity.y -= 8.0f;
	}

	if(glm::length2(entities->dynData[index].velocity) < 9.0f) {
		entities->dynData[index].velocity.x += (3.0f / AISPS) * entities->dynData[index].direction;
		entities->dynData[index].moving = true;
	}
}

void ai_updateArcher(Entities* entities, int index) {
	if(entities->aiData[index].duration > AI_ARCHER_RELOAD_TIME) {
		if(entities->aiData[index].targetID == -1 || entities->components[entities->aiData[index].targetID] == 0)
			entities->aiData[index].targetID = entities->aiData[index].playerID;
		float distance = processXDistance(entities, index, entities->aiData[index].targetID);
		if(distance < entities->aiData[index].aiRange) {
			Vec2 arrowOffset = { entities->collData[index].sz.x / 2, entities->collData[index].sz.y / 4 };
			Vec2 vArrow = processHitDirection(entities, entities->aiData[index].targetID, index);
			entities->dynData[index].direction = (vArrow.x < 0) ? -1 : 1;
			vArrow.y -= processXDistance(entities, index, entities->aiData[index].targetID) / entities->aiData[index].aiRange *0.32f;

			spawnArrow(entities, index, vArrow, arrowOffset);
		} else entities->aiData[index].targetID = entities->aiData[index].playerID;
		entities->aiData[index].duration = 0.0f;
	}
	entities->aiData[index].duration += 1.0f / AISPS;
}

void ai_updateGuard(Entities* entities, int index) {
	if(entities->aiData[index].targetID == -1 || entities->components[entities->aiData[index].targetID] == 0)
		entities->aiData[index].targetID = entities->aiData[index].playerID;
	float distance = processXDistance(entities, index, entities->aiData[index].targetID);
	if(distance < entities->aiData[index].aiRange) {
		float direction = entities->collData[index].pos.x - entities->collData[entities->aiData[index].targetID].pos.x;
		entities->dynData[index].direction = (direction < 0.0f) ? 1 : -1;

		if(entities->dynData[index].touchGround && entities->dynData[index].velocity.x == 0) {
			entities->dynData[index].velocity.y -= 13.0f;
		}

		entities->dynData[index].velocity.x += (15.0f / AISPS) * entities->dynData[index].direction;
		std::clamp(entities->dynData[index].velocity.x, -5.0f, 5.0f);
		entities->dynData[index].moving = true;
	} else {
		entities->dynData[index].moving = false;
		entities->aiData[index].targetID = entities->aiData[index].playerID;
	}
}
