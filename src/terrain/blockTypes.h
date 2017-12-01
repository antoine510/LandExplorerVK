#pragma once

#include "../common.h"
#include "bloc.h"

#define BT_LIQUID    BlocType{0, 0, 0, 1, 0, NULL}

#define BACKWALL_TYPES_OFFSET 256

#define MAX_BLOC_LIGHT 15

struct BlocType {
    Uint8 opacity;
    Uint8 transparency;
	Uint8 atlasOffset;
    bool solid;
    bool liquid;
    bool transparent;

	Uint8 getAtlasOffset(Terrain* terrain, Bloc* bloc) { return atlasOffset + getBlocTypeCustomValue(terrain, bloc); }
    Uint8 (*getBlocTypeCustomValue)(Terrain* terrain, Bloc* bloc);
};

void initBlockTypes(BlocType* blocTypes);
BlocType getType(BlocType* blocTypes, Bloc bloc);
bool checkSolid(BlocType* blocTypes, Bloc bloc);
bool checkLiquid(BlocType* blocTypes, Bloc bloc);
bool checkTransparent(BlocType* blocTypes, Bloc bloc);
bool checkBreakable(BlocType* blocTypes, Bloc bloc);

Uint8 getTransparency(BlocType* blocTypes, Bloc bloc);
bool checkType(BlocType* blocTypes, Bloc bloc, BlocType compareType);
bool checkTypeOR(BlocType* blocTypes, Bloc bloc, BlocType compareType);

