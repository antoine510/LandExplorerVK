#pragma once

#include "../common.h"

typedef struct Label {
    std::string text;
    SDL_Point pos;
} Label;

Label label_create(const std::string& name, SDL_Point pos);
void label_destroy(Label* label);

