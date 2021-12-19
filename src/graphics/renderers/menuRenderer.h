#pragma once

#include "common.h"
#include "ui/menu.h"
#include "utility/xmlTools.h"
#include "graphics/sprite.h"
#include <SDL_ttf.h>

struct SliderTextures {
    Sprite* nameTexture, *valueTexture;
};

struct MenuRenderer {
    TTF_Font* titleFont, *buttonFont, *selectedButtonFont;
    SDL_Color titleColor, buttonColor, selectedButtonColor, labelColor;

	Sprite* titleTexture = nullptr;
	//Sprite* buttonTexture[MAX_MENU_ELEMENT_COUNT];
	//Sprite* labelTexture[MAX_MENU_ELEMENT_COUNT];
    //SliderTextures sliderTextures[MAX_MENU_ELEMENT_COUNT];
	std::vector<Sprite> buttonTextures;
	std::vector<Sprite> labelTextures;
	std::vector<SliderTextures> sliderTextures;
	Sprite* outline = nullptr;
};

void menuRendererLoadTextures(MenuRenderer* menuRenderer, xmlNodePtr mainNode);

void initMenuRenderer(MenuRenderer* menuRenderer);
void renderMenu(Graphics* gfx, Menu* menu);
void destroyMenuRenderer(MenuRenderer* menuRenderer);

