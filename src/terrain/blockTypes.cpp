#include "blockTypes.h"
#include "utility/xmlTools.h"
#include "terrain.h"

static BlocType readBlockType(xmlNodePtr blocNode);

static Uint8 defaultCustomFunction(Terrain* terrain, Bloc* bloc) {return 0;}

void initBlockTypes(BlocType* blocTypes)
{
    xmlDocPtr blocTypesDoc = parseXML("terrain/blocTypes.xml");
	xmlNodePtr bloc = xmlDocGetRootElement(blocTypesDoc)->xmlChildrenNode;
	while(bloc->type == XML_TEXT_NODE) bloc = bloc->next;

	while (bloc)
    {
        int blocID = asIntl(bloc, "id");
		if (checkName(bloc, "Bloc"))
		{
            blocTypes[blocID] = readBlockType(bloc);
		}
		else if(checkName(bloc, "Backwall"))
        {
            blocTypes[blocID + BACKWALL_TYPES_OFFSET] = readBlockType(bloc);
        }

        do bloc = bloc->next; while(bloc && bloc->type == XML_TEXT_NODE);
	}
	xmlFreeDoc(blocTypesDoc);
}

BlocType readBlockType(xmlNodePtr blocNode)
{
    BlocType blocType;
    memset(&blocType, 0, sizeof(BlocType));
    blocType.getBlocTypeCustomValue = defaultCustomFunction;

    blocType.opacity = asIntl(blocNode, "opacity");
	blocType.atlasOffset = asIntl(blocNode, "atlasOffset");

    xmlNodePtr blocElement = blocNode->children;
	while(blocElement->type == XML_TEXT_NODE) blocElement = blocElement->next;

	while (blocElement)
    {
		if (checkName(blocElement, "Solid")) {
            blocType.solid = true;
		} else if(checkName(blocElement, "Liquid")) {
            blocType.liquid = true;
        } else if(checkName(blocElement, "CustomValueFunction")) {
            char* functionName = asStringl(blocElement, "function");
            if(!strcmp(functionName, "edges")) {
                blocType.getBlocTypeCustomValue = getBlocEdges;
            } else if (!strcmp(functionName, "liquid")) {
                blocType.getBlocTypeCustomValue = getLiquidCustomValue;
            }
            free(functionName);
        } else if(checkName(blocElement, "Transparent")) {
            blocType.transparent = true;
            blocType.transparency = asIntl(blocElement, "transparency");
        } else {fprintf(stderr, "XML Error : No bloc type attribute named %s\n", blocElement->name);}

        do blocElement = blocElement->next; while(blocElement && blocElement->type == XML_TEXT_NODE);
	}

	return blocType;
}

BlocType getType(BlocType* blocTypes, Bloc bloc)
{
    return blocTypes[bloc.type];
}

bool checkSolid(BlocType* blocTypes, Bloc bloc)
{
    return blocTypes[bloc.type].solid;
}

bool checkLiquid(BlocType* blocTypes, Bloc bloc)
{
    return blocTypes[bloc.type].liquid;
}

bool checkBreakable(BlocType* blocTypes, Bloc bloc)
{
    return bloc.type != 0 && !checkLiquid(blocTypes, bloc);
}

bool checkTransparent(BlocType* blocTypes, Bloc bloc) {return blocTypes[bloc.type].transparent;}
Uint8 getTransparency(BlocType* blocTypes, Bloc bloc) {return blocTypes[bloc.type].transparency;}

bool checkType(BlocType* blocTypes, Bloc bloc, BlocType compareType)
{
    if(compareType.solid && !checkSolid(blocTypes, bloc)) return false;
    if(compareType.liquid && !checkLiquid(blocTypes, bloc)) return false;
    return true;
}

bool checkTypeOR(BlocType* blocTypes, Bloc bloc, BlocType compareType)
{
    if(compareType.solid && checkSolid(blocTypes, bloc)) return true;
    if(compareType.liquid && checkLiquid(blocTypes, bloc)) return true;
    return false;
}
