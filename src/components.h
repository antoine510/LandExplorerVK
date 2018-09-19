#pragma once

#include "common.h"
#include <glm/vec2.hpp>

using Vec2 = glm::vec2;

#define COMP_RECT               0x00000001
#define COMP_GFXDATA            0x00000002
#define COMP_STATUS             0x00000004
#define COMP_DYNAMIC            0x00000008
#define COMP_PLAYER_CONTROL     0x00000010
#define COMP_AI                 0x00000020
#define COMP_COLLIDEABLE        0x00000040
#define COMP_WEAPON             0x00000080
#define COMP_TRIGGER            0x00000100

typedef struct GfxData {
    int texID;
    unsigned int texState;
    float angle;
} GfxData;

typedef struct DynamicData {
	Vec2 velocity;
    int moving;
    int touchGround;
    int direction;
} DynamicData;

typedef struct CollisionData {
	Vec2 pos;
	Vec2 sz;
    int SAPClientID;
    int collType;
    int collTime;
} CollisionData;

typedef enum AIType {ai_sheep, ai_archer, ai_guard} AIType;

typedef struct AIData {
    AIType aiType;
    float aiRange;
    int targetID, playerID;
    float duration;
} AIData;

typedef struct Status {
    char* name;
    float HP;
    float knockback;
} Status;

typedef enum WeaponType {weapon_sword, weapon_arrow} WeaponType;

typedef struct WeaponData {
    float velocity;
    float knockback;
    int damage;
    WeaponType type;
    int parentID;
    int lifetime;
} WeaponData;

typedef struct TriggerData {
    void* data;
    void (*trigger)(void*);
} TriggerData;

Status createStatus(const std::string& name, float HP, float knockback);
GfxData createGfxData(int texID, unsigned int texState);
DynamicData createDynamicData(Vec2 velocity);
CollisionData createCollisionData(Vec2 pos, Vec2 sz, const std::string& collTypeStr);
AIData createAIData(const std::string& aiTypeStr, float aiRange);
WeaponData createWeaponData(float velocity, float knockback, int damage, const std::string& weaponTypeStr);

void deleteStatus(Status& status);

