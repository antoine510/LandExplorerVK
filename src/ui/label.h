#pragma once

#include "../common.h"
#include "utility/vector.h"

typedef struct Label {
    std::string text;
    SDL_Point pos;
	Vec2 screenOrigin;
} Label;

Label label_create(const std::string& name, SDL_Point pos, Vec2 screenOrigin = Vec2{});
void label_destroy(Label* label);

