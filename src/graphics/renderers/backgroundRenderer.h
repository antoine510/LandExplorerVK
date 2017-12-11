#pragma once

#include "common.h"
#include "utility/xmlTools.h"
#include "graphics/sprite.h"

#define BG_BIOMES 0

#define BG_MENU 16
#define BG_CREATION 17
#define BG_WIN 18

#define MAX_BACKGROUND_COUNT 32

typedef struct BackgroundRenderer {
    Sprite* sun;
	std::array<Sprite*, MAX_BACKGROUND_COUNT> background;

    float* levelTime;
} BackgroundRenderer;

BackgroundRenderer* createBackgroundRenderer();
void backgroundRendererLoadTextures(BackgroundRenderer* bgRenderer, xmlNodePtr mainNode);

inline void setBackgroundRendererTime(BackgroundRenderer* bgRenderer, float* levelTime) { bgRenderer->levelTime = levelTime; }
void renderBackground(BackgroundRenderer* bgRenderer, Graphics* gfx, int bgID);

void destroyBackgroundRenderer(BackgroundRenderer* bgRenderer);

