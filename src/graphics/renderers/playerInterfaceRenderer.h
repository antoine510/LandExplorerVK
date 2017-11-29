#pragma once

#include "common.h"
#include "player/playerController.h"
#include "graphics/sprite.h"

typedef struct PlayerInterfaceRenderer {
    Sprite* deathTextTexture;
} PlayerInterfaceRenderer;

PlayerInterfaceRenderer* createPlayerInterfaceRenderer(Graphics* gfx);
void renderPlayerInterface(Graphics* gfx, PlayerControl* pControl);
void destroyPlayerInterfaceRenderer(PlayerInterfaceRenderer* playerInterfaceRenderer);

