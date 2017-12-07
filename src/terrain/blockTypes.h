#pragma once

#include "common.h"
#include "bloc.h"

#define BACKWALL_TYPES_OFFSET 256

#define MAX_BLOC_LIGHT 15

struct BlocType {
	Uint8(*getBlocTypeCustomValue)(Terrain* terrain, Bloc* bloc);
	bool solid;
	bool liquid;
    Uint8 opacity;
	Uint8 atlasOffset;

	Uint8 getProperAtlasOffset(Terrain* terrain, Bloc* bloc) { return atlasOffset + getBlocTypeCustomValue(terrain, bloc); }
};

void initBlockTypes(BlocType* blocTypes);

inline BlocType getType(BlocType* blocTypes, Bloc bloc) { return blocTypes[bloc.type]; }
inline bool checkSolid(BlocType* blocTypes, Bloc bloc) { return blocTypes[bloc.type].solid; }
inline bool checkLiquid(BlocType* blocTypes, Bloc bloc) { return blocTypes[bloc.type].liquid; }
inline bool checkBreakable(BlocType* blocTypes, Bloc bloc) { return bloc.type != 0 && !checkLiquid(blocTypes, bloc); }

