#pragma once

#include "common.h"
#include "graphics/graphics.h"
#include "sound.h"
#include "keyStates.h"
#include "ui/menu.h"
#include "map.h"
#include "level.h"
#include "editor.h"

typedef struct LandExplorer {
    Graphics* gfx;
    Sound* sound;
    KeyStates keyStates;
    Level* level;
    Menu* menu;
    Map* mapp;
    Editor* editor;
    Credits* credits;

    ModeUpdateResult mode;
} LandExplorer;

void initExplorer(LandExplorer* landExplorer);
void mainLoop(LandExplorer* landExplorer);
void destroyExplorer(LandExplorer* landExplorer);

