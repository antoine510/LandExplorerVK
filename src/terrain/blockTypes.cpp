#include "blockTypes.h"
#include "utility/xmlTools.h"
#include "terrain.h"

static BlocType readBlockType(xmlNodePtr blocNode);

static Uint8 defaultCustomFunction(Terrain* terrain, Bloc* bloc) { return 0; }

void initBlockTypes(BlocType* blocTypes) {
	xmlDocPtr blocTypesDoc = parseXML("terrain/blocTypes.xml");
	xmlNodePtr bloc = xmlDocGetRootElement(blocTypesDoc)->xmlChildrenNode;
	while(bloc->type == XML_TEXT_NODE) bloc = bloc->next;

	while(bloc) {
		int blocID = asIntl(bloc, "id");
		if(checkName(bloc, "Bloc")) {
			blocTypes[blocID] = readBlockType(bloc);
		} else if(checkName(bloc, "Backwall")) {
			blocTypes[blocID + BACKWALL_TYPES_OFFSET] = readBlockType(bloc);
		}

		do bloc = bloc->next; while(bloc && bloc->type == XML_TEXT_NODE);
	}
	xmlFreeDoc(blocTypesDoc);
}

BlocType readBlockType(xmlNodePtr blocNode) {
	BlocType blocType;
	memset(&blocType, 0, sizeof(BlocType));
	blocType.getBlocTypeCustomValue = defaultCustomFunction;

	blocType.opacity = asIntl(blocNode, "lightOpacity");
	blocType.atlasOffset = asIntl(blocNode, "atlasOffset");

	xmlNodePtr blocElement = blocNode->children;
	SKIP_TEXT(blocElement);

	while(blocElement) {
		if(checkName(blocElement, "Solid")) {
			blocType.solid = true;
		} else if(checkName(blocElement, "Liquid")) {
			blocType.liquid = true;
		} else if(checkName(blocElement, "CustomValueFunction")) {
			char* functionName = asStringl(blocElement, "function");
			if(!strcmp(functionName, "edges")) {
				blocType.getBlocTypeCustomValue = getBlocEdges;
			} else if(!strcmp(functionName, "liquid")) {
				blocType.getBlocTypeCustomValue = getLiquidCustomValue;
			}
			free(functionName);
		} else { fprintf(stderr, "XML Error : No bloc type attribute named %s\n", blocElement->name); }

		blocElement = blocElement->next;
		SKIP_TEXT(blocElement);
	}

	return blocType;
}

