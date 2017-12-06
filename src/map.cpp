#include "map.h"
#include "terrain/terrain.h"
#include "terrain/blockTypes.h"
#include "graphics/displayInfo.h"
#include <algorithm>

Map* initMap() {
	Map* mapp = (Map*)calloc(1, sizeof(Map));

	mapp->mapUpdated = 0;
	mapp->scalingFactor = 2;

	return mapp;
}

ModeUpdateResult updateMap(Map* mapp, KeyStates* keyStates) {
	int x, y;
	SDL_GetMouseState(&x, &y);

	if(isKeyHeld(keyStates, key_map)) return MURESULT_LEVEL;
	int oldScalingFactor = mapp->scalingFactor;
	mouseWheelIncrement(&mapp->scalingFactor);
	mapp->scalingFactor = std::clamp(mapp->scalingFactor, -2, 3);
	if(oldScalingFactor != mapp->scalingFactor) {
		mapp->panning = 0;
		scaleMap(mapp, mapp->scalingFactor - oldScalingFactor, x, y);
	}
	if(isMouseLeftDown()) {
		mapp->panning = 1; mapp->startPanningPos = SDL_Point{ x + mapp->panningPos.x, y + mapp->panningPos.y };
	} else if(isMouseLeftUp()) mapp->panning = 0;

	if(mapp->panning) {
		mapp->panningPos.x = mapp->startPanningPos.x - x;
		mapp->panningPos.y = mapp->startPanningPos.y - y;
	}

	return MURESULT_MAP;
}

void scaleMap(Map* mapp, int deltaScaling, int centerX, int centerY) {
	if(deltaScaling >= 0) {
		//If we zoom we have to keep the panning pos constant
		mapp->panningPos.x = mapp->panningPos.x * (1 << deltaScaling) + centerX;
		mapp->panningPos.y = mapp->panningPos.y * (1 << deltaScaling) + centerY;
	} else {
		//The same if we de-zoom
		mapp->panningPos.x = (mapp->panningPos.x - centerX) / (1 << -deltaScaling);
		mapp->panningPos.y = (mapp->panningPos.y - centerY) / (1 << -deltaScaling);
	}
}

void destroyMap(Map* mapp) {
	free(mapp);
}
