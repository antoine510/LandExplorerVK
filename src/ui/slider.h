#pragma once

#include "../common.h"

#define SLIDER_PLUS(x) (x*2 +1)
#define SLIDER_MINUS(x) (x*2)

typedef struct Slider {
    char* name;
    SDL_Point pos;
    int value, min, max;
} Slider;

Slider* slider_create(char* name, SDL_Point pos, int value, int min, int max);

void slider_changeValue(Slider* slider, int change);

SDL_Rect slider_getMinusRect(Slider* slider);
SDL_Rect slider_getPlusRect(Slider* slider);

void slider_destroy(Slider* slider);

