#include "buildingGen.h"
#include "utility/mathUtility.h"
#include "../level.h"
#include "heightProfile.h"
#include "utility/random.h"

#define MAX_STRUCTURE_BUILDINGS 16

typedef enum BuildingTypes {
	House1, House2, Castle_Trench, Castle_Dungeon, Castle_Tower, Castle_House,
	BuildingTypeCount
} BuildingTypes;

static void genGround(Terrain* terrain, int xStart, int xEnd, int thickness, int type, int backwallType);
static void genGroundBeneath(Terrain* terrain, int xStart, int xEnd, int buildingLevel, int type, int backwallType);
static void generateBuilding(Level* level, Module* module, int xBuilding, bool mirror);
static int genSmallVillage(Level* level, Module** modules, int xStart);
static int genVillage(Level* level, Module** modules, int xStart);
static int genVille(Level* level, Module** modules, int xStart);

void genGround(Terrain* terrain, int xStart, int xEnd, int thickness, int type, int backwallType) {
	int x, i;
	for(x = xStart; x < xEnd; x++) {
		int terrainHeight = getTerrainHeight(terrain, x);
		for(i = 1; i <= thickness; i++) {
			getBlockPtr(terrain, x, terrainHeight + i)->type = type;
			getBackwallPtr(terrain, x, terrainHeight + i + 4)->type = backwallType;
		}
	}
	//printf("Creating ground : thickness = %d, start = %d, end = %d, type = %d\n", thickness, xStart, xEnd, type);
}

void genGroundBeneath(Terrain* terrain, int xStart, int xEnd, int buildingLevel, int type, int backwallType) {
	int x, y;
	for(x = xStart; x < xEnd; x++) {
		int terrainHeight = getTerrainHeight(terrain, x);
		for(y = buildingLevel + 1; y <= terrainHeight; y++) {
			getBlockPtr(terrain, x, y)->type = type;
			getBackwallPtr(terrain, x, y + 4)->type = backwallType;
		}
	}
	//printf("Creating ground beneath building : start = %d, end = %d, type = %d\n", xStart, xEnd, type);
}

int genSmallVillage(Level* level, Module** modules, int xStart) {
	const int moduleCount = 3;
	int xBuilding[MAX_STRUCTURE_BUILDINGS];
	BuildingTypes buildingType[MAX_STRUCTURE_BUILDINGS];
	int currentX = xStart;

	int i;
	for(i = 0; i < moduleCount; i++) {
		buildingType[i] = (BuildingTypes)Random<int>::range(0, 1);
		xBuilding[i] = currentX;
		currentX += modules[buildingType[i]]->size.x + 10 + buildingType[i];
	}

	genGround(level->terrain, xStart, currentX, 2, 2, 2);

	for(i = 0; i < moduleCount; i++) {
		generateBuilding(level, modules[buildingType[i]], xBuilding[i], false);
	}

	return currentX;
}

int genVillage(Level* level, Module** modules, int xStart) {
	const int moduleCount = 5;
	int xBuilding[MAX_STRUCTURE_BUILDINGS];
	BuildingTypes buildingType[MAX_STRUCTURE_BUILDINGS];
	int currentX = xStart;

	int i;
	for(i = 0; i < moduleCount; i++) {
		buildingType[i] = (BuildingTypes)Random<int>::range(0, 2);
		xBuilding[i] = currentX;
		currentX += modules[buildingType[i]]->size.x + 10 + buildingType[i];
	}

	genGround(level->terrain, xStart, currentX, 3, 2, 2);

	for(i = 0; i < 5; i++) {
		generateBuilding(level, modules[buildingType[i]], xBuilding[i], false);
	}

	return currentX;
}

int genVille(Level* level, Module** modules, int xStart) {
	const int moduleCount = Random<int>::range(10, 14);
	int xBuilding[MAX_STRUCTURE_BUILDINGS];
	BuildingTypes buildingType[MAX_STRUCTURE_BUILDINGS];
	int currentX = xStart;

	int i;
	for(i = 0; i < moduleCount; i++) {
		buildingType[i] = (BuildingTypes)Random<int>::range(0, 2);     //Change this to modify the accessible modules
		xBuilding[i] = currentX;
		currentX += modules[buildingType[i]]->size.x + 10 + buildingType[i];
	}

	genGround(level->terrain, xStart, currentX, 5, 2, 2);

	for(i = 0; i < moduleCount; i++) {
		generateBuilding(level, modules[buildingType[i]], xBuilding[i], false);
	}

	return currentX;
}

int genCastle(Level* level, Module** modules, int xStart) {
	const int moduleCountPrevious = Random<int>::range(1, 3), moduleCountAfter = Random<int>::range(1, 3);
	const int moduleCount = 3 + moduleCountPrevious + moduleCountAfter;
	int xBuilding[MAX_STRUCTURE_BUILDINGS];
	BuildingTypes buildingType[MAX_STRUCTURE_BUILDINGS];
	int currentX = xStart;

	buildingType[0] = Castle_Tower;
	xBuilding[0] = currentX;
	currentX += modules[buildingType[0]]->size.x + 10;

	int i;
	for(i = 1; i < 1 + moduleCountPrevious; i++) {
		buildingType[i] = Castle_House;
		xBuilding[i] = currentX;
		currentX += modules[buildingType[i]]->size.x + 10;
	}

	buildingType[1 + moduleCountPrevious] = Castle_Dungeon;
	xBuilding[1 + moduleCountPrevious] = currentX;
	currentX += modules[buildingType[1 + moduleCountPrevious]]->size.x + 10;

	for(i = 2 + moduleCountPrevious; i < 2 + moduleCountPrevious + moduleCountAfter; i++) {
		buildingType[i] = Castle_House;
		xBuilding[i] = currentX;
		currentX += modules[buildingType[i]]->size.x + 10;
	}

	buildingType[2 + moduleCountPrevious + moduleCountAfter] = Castle_Tower;
	xBuilding[2 + moduleCountPrevious + moduleCountAfter] = currentX;
	currentX += modules[buildingType[2 + moduleCountPrevious + moduleCountAfter]]->size.x + 10;

	genGround(level->terrain, xStart, currentX, 5, 2, 2);

	for(i = 0; i < moduleCount - 1; i++) {
		generateBuilding(level, modules[buildingType[i]], xBuilding[i], false);
	}
	generateBuilding(level, modules[buildingType[moduleCount - 1]], xBuilding[moduleCount - 1], true);

	return currentX;
}

void generateBuilding(Level* level, Module* module, int xBuilding, bool mirror) {
	//printf("GenererModule %d : x = %d, y = sol \n", moduleID[i], xBuilding[i]);
	genGroundBeneath(level->terrain, xBuilding, xBuilding + module->size.x, getTerrainHeight(level->terrain, xBuilding), 2, 2);
	pasteModule(module, level, SDL_Point{ xBuilding + module->ref.x, getTerrainHeight(level->terrain, xBuilding) }, false, mirror);
}

void generateBuildings(Level* level) {
	Terrain* terrain = level->terrain;

	Module* modules[BuildingTypeCount];
	modules[House1] = loadModule("maisonA");
	modules[House2] = loadModule("maisonB");
	modules[Castle_Trench] = loadModule("douve");
	modules[Castle_Dungeon] = loadModule("dungeon");
	modules[Castle_Tower] = loadModule("tower");
	modules[Castle_House] = loadModule("castleHouse");

	int currentX = 150, moduleSelection;
	while(currentX < terrain->width - 400) {
		moduleSelection = Random<int>::range(0, 100);
		if(moduleSelection < 50 && currentX < PLAINS_LIMIT)        //50% chance
		{
			currentX = genSmallVillage(level, modules, currentX) + Random<int>::range(60, 100);
		} else if(moduleSelection < 80 && currentX < PLAINS_LIMIT)   //30% chance
		{
			if(currentX > PLAINS_LIMIT)
				currentX = genCastle(level, modules, currentX) + Random<int>::range(100, 180);
			else
				currentX = genVillage(level, modules, currentX) + Random<int>::range(100, 180);
		} else                            //20% chance
		{
			if(currentX > PLAINS_LIMIT)
				currentX = genCastle(level, modules, currentX) + Random<int>::range(100, 200);
			else
				currentX = genVille(level, modules, currentX) + Random<int>::range(100, 200);
		}
	}

	for(int i = 0; i < BuildingTypeCount; ++i) {
		destroyModule(modules[i]);
	}
}
