#pragma once

#include "common.h"
#include "ui/credits.h"
#include "graphics/sprite.h"
#include <SDL_ttf.h>

typedef struct CreditsRenderer {
	Sprite* lineTextures[MAX_CREDIT_LINES];
	Sprite* endLineTexture;
} CreditsRenderer;

CreditsRenderer* createCreditsRenderer();
void initCreditsRenderer(Credits* credits, Graphics* gfx);
void renderCredits(Graphics* gfx, Credits* credits);
void destroyCreditsRenderer(CreditsRenderer* creditsRenderer);

