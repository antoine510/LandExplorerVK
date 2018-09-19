#pragma once

#include "../common.h"
#include "utility/vector.h"

#define MAX_BUTTON_NAME_LENGTH 1024

#define BUTTON_UNSELECTED 0
#define BUTTON_SELECTED 1

typedef struct Button {
    std::string name;
    SDL_Rect rect;
	Vec2 screenOrigin;
    int state;
} Button;

Button createButton(SDL_Rect rect, std::string name, Vec2 screenOrigin = Vec2{});

