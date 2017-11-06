#include "xmlTools.h"

static const char* getAttribute(xmlNodePtr node, const char* attribName);

xmlDocPtr parseXML(const std::string& filename)
{
    xmlDocPtr doc = xmlParseFile(filename.c_str());

	SDL_assert_always(doc != NULL);

	return doc;
}

const char* getAttribute(xmlNodePtr node, const char* attribName)
{
    const xmlChar* attrib = xmlGetProp(node, (const xmlChar *)attribName);
    return (const char*)attrib;
}

bool checkName(xmlNodePtr node, const char* name)
{
    return !xmlStrcmp(node->name, (const xmlChar *)name);
}

char* asStringl(xmlNodePtr node, const char* attribName)
{
    const char* src = getAttribute(node, attribName);
    SDL_assert(src != NULL);
    char* dst = (char*)malloc(strlen(src)+sizeof(char));
    return strcpy(dst, src);
}

int asIntl(xmlNodePtr node, const char* attribName)
{
    const char* attribute = getAttribute(node, attribName);
    return (attribute != NULL) ? atoi(attribute) : 0;
}

float asFloatl(xmlNodePtr node, const char* attribName)
{
    const char* attribute = getAttribute(node, attribName);
    return (attribute != NULL) ? (float)atof(attribute) : 0.0f;
}

int asBooll(xmlNodePtr node, const char* attribName)
{
    return getAttribute(node, attribName)[0] == 't';
}

bool checkAttribute(xmlNodePtr node, const char* attribName, const char* compare)
{
    return !xmlStrcmp((const xmlChar *)getAttribute(node, attribName), (const xmlChar *)compare);
}

bool checkAttributeExists(xmlNodePtr node, const char* attribName)
{
    return (getAttribute(node, attribName) != NULL);
}

char* getNodeContent(xmlNodePtr node)
{
    const char* src = (const char*)xmlNodeGetContent(node);
    char* dst = (char*)malloc(strlen(src)+sizeof(char));
    return strcpy(dst, src);
}

SDL_Rect getRectl(xmlNodePtr node)
{
    return SDL_Rect{asIntl(node, "X"), asIntl(node, "Y"), asIntl(node, "W"), asIntl(node, "H")};
}

SDL_Point getPosl(xmlNodePtr node)
{
    return SDL_Point{asIntl(node, "X"), asIntl(node, "Y")};
}

Status getStatusl(xmlNodePtr node)
{
    return createStatus(asStringl(node, "name"), asFloatl(node, "HP"), asFloatl(node, "knockback"));
}

GfxData getGfxDatal(xmlNodePtr node)
{
    return createGfxData(asIntl(node, "texID"), asIntl(node, "texState"));
}

DynamicData getDynamicData(xmlNodePtr node)
{
    return createDynamicData(Vec2{asFloatl(node, "vx"), asFloatl(node, "vy")});
}

CollisionData getCollisionData(xmlNodePtr node)
{
    return createCollisionData(Vec2{asFloatl(node, "x"), asFloatl(node, "y")},
                               Vec2{asFloatl(node, "w"), asFloatl(node, "h")},
                               asStringl(node, "collType"));
}

AIData getAIData(xmlNodePtr node)
{
    return createAIData(asStringl(node, "aiType"), asFloatl(node, "aiRange"));
}

WeaponData getWeaponData(xmlNodePtr node)
{
    return createWeaponData(asFloatl(node, "velocity"), asFloatl(node, "knockback"), asIntl(node, "damage"), asStringl(node, "type"));
}

