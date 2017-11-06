#include "entityModels.h"
#include "utility/xmlTools.h"

void setEntityAsModel(Entities* entities, unsigned int entityID, const char* modelName) {
	destroyEntity(entities, entityID);

	std::string filename("entityModels/");
	filename += modelName;
	filename += ".xml";

	xmlDocPtr levelDoc = parseXML(filename);
	xmlNodePtr component = xmlDocGetRootElement(levelDoc)->xmlChildrenNode;
	while(component->type == XML_TEXT_NODE) component = component->next;

	while(component) {
		if(checkName(component, "Rect")) {
			entities->components[entityID] |= COMP_RECT;
			entities->rect[entityID] = getRectl(component);
		} else if(checkName(component, "GfxData")) {
			entities->components[entityID] |= COMP_GFXDATA;
			entities->gfxData[entityID] = getGfxDatal(component);
		} else if(checkName(component, "Status")) {
			entities->components[entityID] |= COMP_STATUS;
			entities->status[entityID] = getStatusl(component);
		} else if(checkName(component, "DynamicData")) {
			entities->components[entityID] |= COMP_DYNAMIC;
			entities->dynData[entityID] = getDynamicData(component);
		} else if(checkName(component, "CollisionData")) {
			entities->components[entityID] |= COMP_COLLIDEABLE;
			entities->collData[entityID] = getCollisionData(component);
			initCollisionData(entities, entityID);
		} else if(checkName(component, "AIData")) {
			entities->components[entityID] |= COMP_AI;
			entities->aiData[entityID] = getAIData(component);
		} else if(checkName(component, "PlayerControl")) {
			entities->components[entityID] |= COMP_PLAYER_CONTROL;
		} else if(checkName(component, "WeaponData")) {
			entities->components[entityID] |= COMP_WEAPON;
			entities->weaponData[entityID] = getWeaponData(component);
		} else if(checkName(component, "TriggerData")) {
			entities->components[entityID] |= COMP_TRIGGER;
		} else { fprintf(stderr, "Error : Unexpected component in model %s\n", modelName); }

		do component = component->next; while(component && component->type == XML_TEXT_NODE);
	}
	xmlFreeDoc(levelDoc);
}

