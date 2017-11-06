#pragma once

#include "common.h"
#include "libxml/parser.h"
#include "components.h"

xmlDocPtr parseXML(const std::string& filename);
bool checkName(xmlNodePtr node, const char* name);
char* asStringl(xmlNodePtr node, const char* attribName);
int asIntl(xmlNodePtr node, const char* attribName);
float asFloatl(xmlNodePtr node, const char* attribName);
int asBooll(xmlNodePtr node, const char* attribName);

bool checkAttribute(xmlNodePtr node, const char* attribName, const char* compare);
bool checkAttributeExists(xmlNodePtr node, const char* attribName);
char* getNodeContent(xmlNodePtr node);

SDL_Rect getRectl(xmlNodePtr node);
SDL_Point getPosl(xmlNodePtr node);
Status getStatusl(xmlNodePtr node);
GfxData getGfxDatal(xmlNodePtr node);
DynamicData getDynamicData(xmlNodePtr node);
CollisionData getCollisionData(xmlNodePtr node);
AIData getAIData(xmlNodePtr node);
WeaponData getWeaponData(xmlNodePtr node);
