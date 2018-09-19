#include "components.h"
#include "physics/collisions.h"

Status createStatus(const std::string& name, float HP, float knockback)
{
	char* dst = (char*)malloc(name.size() + 1);
	strcpy_s(dst, name.size() + 1, name.c_str());
	return Status{dst, HP, knockback};
}

GfxData createGfxData(int texID, unsigned int texState)
{
	return GfxData{texID, texState, 0.0f};
}

WeaponData createWeaponData(float velocity, float knockback, int damage, const std::string& weaponTypeStr)
{
    WeaponType type = weapon_sword;
    if(weaponTypeStr == "sword") type = weapon_sword;
    else if (weaponTypeStr == "arrow") type = weapon_arrow;
	else throw std::runtime_error("Weapon type '" + weaponTypeStr + "' does not exist");

	return WeaponData{velocity, knockback, damage, type, -1, 0};
}

DynamicData createDynamicData(Vec2 velocity)
{
    return DynamicData{velocity, 0, 0, 1};
}

AIData createAIData(const std::string& aiTypeStr, float aiRange)
{
    AIType type = ai_sheep;
    if(aiTypeStr == "sheep") type = ai_sheep;
    else if (aiTypeStr == "archer") type = ai_archer;
    else if (aiTypeStr == "guard") type = ai_guard;
	else throw std::runtime_error("AI type '" + aiTypeStr + "' does not exist");

    return AIData{type, aiRange, -1, -1, 0.0f};
}

CollisionData createCollisionData(Vec2 pos, Vec2 sz, const std::string& collTypeStr)
{
    int collType = COLLTYPE_NOCOLL;
    if(collTypeStr == "player") collType = COLLTYPE_PLAYER;
    else if (collTypeStr == "passive") collType = COLLTYPE_PASSIVE;
    else if (collTypeStr == "melee") collType = COLLTYPE_MELEE;
    else if(collTypeStr == "weapon") collType = COLLTYPE_WEAPON;
    else if(collTypeStr == "trigger") collType = COLLTYPE_TRIGGER;
	else throw std::runtime_error("Collision type '" + collTypeStr + "' does not exist");

    return CollisionData{pos, sz, 0, collType, 0};
}

void deleteStatus(Status& status) {
	if(status.name != nullptr) free(status.name);
}
