#pragma once

#include "oglCommon.h"
#include "renderer.h"
#include "framebuffer.h"

#include "../texturePack.h"
#include "../../terrain/terrain.h"

typedef struct ChunckBuilder {
    FrameBuffer blocBuffer, backwallBuffer;
    bool backwall;

    TexturePack* texPack;
    Texture* blocTex;
    Texture* backwallTex;
} ChunckBuilder;

ChunckBuilder* chb_create(Texture* blocTex, Texture* backwallTex, TexturePack* texPack);

void chb_buildRect(ChunckBuilder* builder, Terrain* terrain, char* textureIDs, SDL_Rect* rect);
void chb_startBuilding(ChunckBuilder* builder, bool backwall);
void chb_endBuilding();

void chb_destroy(ChunckBuilder* builder);

