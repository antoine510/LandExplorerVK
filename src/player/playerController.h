#pragma once

#include "../common.h"
#include "../keyStates.h"
#include "inventory.h"
#include "../ui/layout.h"

#define MAX_SIDEWARD_VELOCITY (9.0f) //(15.0f)
#define SIDEWARD_ACCELERATION (25.0f) //(30.0f)
#define JUMP_VELOCITY (20.5f) //(20.0f)

#define BLOC_BREAKING_STEPS 4
#define BLOC_BREAKING_TIME (0.07f * PCPS);

#define HEALTH_PER_HEARTH 20
#define HEALTH_REGEN_PER_SECOND 2.0f
#define MAX_PLAYER_HP 100.0f
#define PLAYER_DEATH_LENGTH 5000

struct PlayerControl {
    Inventory* inventory;
    bool inventoryActive;
    int cursorItemID;
    int cursorItemCount;
    Layout* inventoryLayout;

	Vec4* viewOrigin;
    float* playerLife;
    int currentWeaponID;
    int direction;
    int breakTimer;
    int deathTimerID;
    bool isDead;

    int breaking;
    Uint32 breakingX, breakingY;
    bool isUsing;
    int usingTimerID;
};

PlayerControl* initPlayerControl(Vec4* viewOrigin, Entities* entities, int playerID);
void updateController(KeyStates* keyStates, Level* level, int playerID, PlayerControl* pControl);
void destroyPlayerControl(PlayerControl* pControl);

