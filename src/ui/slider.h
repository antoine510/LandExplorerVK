#pragma once

#include "../common.h"
#include "utility/vector.h"

#define SLIDER_PLUS(x) (x*2 +1)
#define SLIDER_MINUS(x) (x*2)

typedef struct Slider {
    std::string name;
    SDL_Point pos;
	Vec2 screenOrigin;
    int value, min, max;
} Slider;

Slider slider_create(std::string name, SDL_Point pos, int value, int min, int max, Vec2 screenOrigin = Vec2{});

void slider_changeValue(Slider& slider, int change);

SDL_Rect slider_getMinusRect(const Slider& slider);
SDL_Rect slider_getPlusRect(const Slider& slider);

