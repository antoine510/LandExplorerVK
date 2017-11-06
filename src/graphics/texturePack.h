#pragma once

#include "common.h"
#include "components.h"
#include "terrain/terrain.h"
#include "textureSet.h"
#include "vulkan/Image.h"

#define MAX_TEXTURESET_COUNT 10
#define MAX_BUTTONSET_COUNT 10

#define TEXTURE_ITEM_SIZE 32

typedef struct TexturePack {
	StagedImage* blocAtlas;
	StagedImage* backwallAtlas;

    Sprite* breakingBloc;
	Sprite* playerHearth;
	Sprite* itemSlot;
	Sprite* itemAtlas;
    TextureSet* texSets[MAX_TEXTURESET_COUNT];
    TextureSet* buttonSets[MAX_BUTTONSET_COUNT];

    SDL_Color skyColor;
} TexturePack;


TexturePack* initTexturePack(Graphics* gfx);

void blitBreak(TexturePack* texPack, int breakingStep, Uint32 x, Uint32 y);
void blitMapIcon(TexturePack* texPack, GfxData* gfxData, SDL_Rect rect, float scalingFactor, SDL_Point panningPos);
void blitEntity(TexturePack* texPack, GfxData* gfxData, SDL_Rect rect);
void blitHearth(TexturePack* texPackk, SDL_Point pos, float ratioFull);
void blitSlot(TexturePack* texPack, SDL_Point pos, float sizeMul);
void blitItem(TexturePack* texPack, SDL_Point pos, int itemID);
void texPackFrameReset(TexturePack* texPack);

void destroyTexturePack(TexturePack* texPack);

