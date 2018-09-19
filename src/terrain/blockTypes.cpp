#include "blockTypes.h"
#include "luaScript.h"
#include "terrain.h"

static BlocType readBlockType(LuaScript& script);

static Uint8 defaultCustomFunction(Terrain* terrain, Bloc* bloc) { return 0; }

void initBlockTypes(BlocType* blocTypes) {
	LuaScript script("terrain/blocTypes.lua");

	{
		auto scope(script.getScope("blocs"));
		int blocCount = script.getLength();
		for(int i = 0; i < blocCount; ++i) {
			auto scope(script.getScope(i + 1));	// Lua arrays are indexed from 1
			blocTypes[i] = readBlockType(script);
		}
	}
	{
		auto scope(script.getScope("backwalls"));
		int backwallCount = script.getLength();
		for(int i = 0; i < backwallCount; ++i) {
			auto scope(script.getScope(i + 1));
			blocTypes[i + BACKWALL_TYPES_OFFSET] = readBlockType(script);
		}
	}
}

BlocType readBlockType(LuaScript& script) {
	BlocType blocType;
	memset(&blocType, 0, sizeof(BlocType));
	blocType.getBlocTypeCustomValue = defaultCustomFunction;

	blocType.opacity = script.get<int>("lightOpacity");
	blocType.atlasOffset = script.get<int>("atlasOffset");
	blocType.solid = script.hasTrue("solid");
	blocType.liquid = script.hasTrue("liquid");
	if(script.has("cvFunction")) {
		auto cvf = script.get<std::string>("cvFunction");
		if(cvf == "edges") {
			blocType.getBlocTypeCustomValue = getBlocEdges;
		} else if(cvf == "liquid") {
			blocType.getBlocTypeCustomValue = getLiquidCustomValue;
		}
	}

	return blocType;
}

