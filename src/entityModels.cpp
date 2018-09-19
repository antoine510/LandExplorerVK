#include "entityModels.h"
#include "luaScript.h"

void setEntityAsModel(Entities* entities, unsigned int entityID, const std::string& modelName) {
	destroyEntity(entities, entityID);

	LuaScript script("entityModels/" + modelName + ".lua");
	if(script.has("size")) {
		auto scope(script.getScope("size"));
		entities->components[entityID] |= COMP_RECT;
		auto sz = script.get<SDL_Point>();
		entities->rect[entityID] = {0, 0, sz.x, sz.y};
	}
	if(script.has("gfxData")) {
		auto scope(script.getScope("gfxData"));
		entities->components[entityID] |= COMP_GFXDATA;
		entities->gfxData[entityID] = createGfxData(script.get<int>("texID"), script.get<int>("texState"));
	}
	if(script.has("status")) {
		auto scope(script.getScope("status"));
		entities->components[entityID] |= COMP_STATUS;
		entities->status[entityID] = createStatus(script.get<std::string>("name"), script.get<float>("HP"), script.get<float>("knockback"));
	}
	if(script.hasTrue("dynamic")) {
		entities->components[entityID] |= COMP_DYNAMIC;
		entities->dynData[entityID] = createDynamicData(Vec2());
	}
	if(script.has("collisionData")) {
		auto scope(script.getScope("collisionData"));
		entities->components[entityID] |= COMP_COLLIDEABLE;
		entities->collData[entityID] = createCollisionData(Vec2(), script.get<Vec2>(), script.get<std::string>("collType"));
		initCollisionData(entities, entityID);
	}
	if(script.has("AIData")) {
		auto scope(script.getScope("AIData"));
		entities->components[entityID] |= COMP_AI;
		entities->aiData[entityID] = createAIData(script.get<std::string>("AIType"), script.get<float>("AIRange"));
	}
	if(script.hasTrue("playerControl")) entities->components[entityID] |= COMP_PLAYER_CONTROL;
	if(script.has("weaponData")) {
		auto scope(script.getScope("weaponData"));
		entities->components[entityID] |= COMP_WEAPON;
		entities->weaponData[entityID] = createWeaponData(script.get<float>("velocity"), script.get<float>("knockback"), script.get<int>("damage"), script.get<std::string>("type"));
	}
	if(script.hasTrue("trigger")) entities->components[entityID] |= COMP_TRIGGER;
}

