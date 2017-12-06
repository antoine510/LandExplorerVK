#pragma once

#include "../common.h"
#include "utility/vector.h"

#define MAX_BUTTON_NAME_LENGTH 1024

#define BUTTON_UNSELECTED 0
#define BUTTON_SELECTED 1

typedef struct Button {
    char name[MAX_BUTTON_NAME_LENGTH];
    SDL_Rect rect;
	Vec2 screenOrigin;
    int state;
} Button;

Button* createButton(SDL_Rect rect, const char* name, Vec2 screenOrigin = Vec2{});
void destroyButton(Button* button);

