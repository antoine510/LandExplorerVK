#include "components.h"
#include "physics/collisions.h"

Status createStatus(char* name, float HP, float knockback)
{
	return Status{name, HP, knockback};
}

GfxData createGfxData(int texID, unsigned int texState)
{
	return GfxData{texID, texState, 0.0f};
}

WeaponData createWeaponData(float velocity, float knockback, int damage, const char* weaponTypeStr)
{
    WeaponType type = weapon_sword;
    if(!strcmp(weaponTypeStr, "sword")) type = weapon_sword;
    else if (!strcmp(weaponTypeStr, "arrow")) type = weapon_arrow;
    else fprintf(stderr, "Error : Weapon type %s doesn't exist\n", weaponTypeStr);

	return WeaponData{velocity, knockback, damage, type, -1, 0};
}

DynamicData createDynamicData(Vec2 velocity)
{
    return DynamicData{velocity, 0, 0, 1};
}

AIData createAIData(const char* aiTypeStr, float aiRange)
{
    AIType type = ai_sheep;
    if(!strcmp(aiTypeStr, "sheep")) type = ai_sheep;
    else if (!strcmp(aiTypeStr, "archer")) type = ai_archer;
    else if (!strcmp(aiTypeStr, "guard")) type = ai_guard;
    else fprintf(stderr, "Error : AI type %s doesn't exist\n", aiTypeStr);
    return AIData{type, aiRange, -1, -1, 0.0f};
}

CollisionData createCollisionData(Vec2 pos, Vec2 sz, const char* collTypeStr)
{
    int collType = COLLTYPE_NOCOLL;
    if(!strcmp(collTypeStr, "player")) collType = COLLTYPE_PLAYER;
    else if (!strcmp(collTypeStr, "passive")) collType = COLLTYPE_PASSIVE;
    else if (!strcmp(collTypeStr, "melee")) collType = COLLTYPE_MELEE;
    else if(!strcmp(collTypeStr, "weapon")) collType = COLLTYPE_WEAPON;
    else if(!strcmp(collTypeStr, "trigger")) collType = COLLTYPE_TRIGGER;
    else fprintf(stderr, "Error : Collision type %s doesn't exist\n", collTypeStr);

    return CollisionData{pos, sz, 0, collType, 0};
}

void deleteStatus(Status status)
{
    free(status.name);
}
