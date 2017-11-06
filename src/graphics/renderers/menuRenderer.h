#pragma once

#include "common.h"
#include "ui/menu.h"
#include "utility/xmlTools.h"
#include "graphics/sprite.h"
#include <SDL_ttf.h>

typedef struct SliderTextures {
    Sprite* nameTexture, *valueTexture;
} SliderTextures;

typedef struct MenuRenderer {
    TTF_Font* titleFont, *buttonFont, *selectedButtonFont;
    SDL_Color titleColor, buttonColor, selectedButtonColor, labelColor;

	Sprite* titleTexture;
	Sprite* buttonTexture[MAX_MENU_ELEMENT_COUNT];
	Sprite* selectedButtonTexture;
	Sprite* labelTexture[MAX_MENU_ELEMENT_COUNT];
    SliderTextures sliderTextures[MAX_MENU_ELEMENT_COUNT];
	Sprite* outline;
} MenuRenderer;

void menuRendererLoadTextures(MenuRenderer* menuRenderer, xmlNodePtr mainNode);

void initMenuRenderer(MenuRenderer* menuRenderer);
void renderMenu(Graphics* gfx, Menu* menu);
void destroyMenuRenderer(MenuRenderer* menuRenderer);

