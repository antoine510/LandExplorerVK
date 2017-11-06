#pragma once

#include "common.h"
#include "button.h"
#include "label.h"
#include "slider.h"
#include "../keyStates.h"
#include "layout.h"
#include "../modeUpdateResult.h"

#define MAX_MENU_ELEMENT_COUNT 16

#define SUBMENU_MAIN_MENU 0
#define SUBMENU_OPTIONS 1
#define SUBMENU_GRAPHIC_OPTIONS 2
#define SUBMENU_AUDIO_OPTIONS 3
#define SUBMENU_KEY_BINDINGS 4
#define SUBMENU_INSTRUCTIONS 5
#define SUBMENU_COUNT 6

typedef struct SubMenu {
    char* name;
    Button* buttons[MAX_MENU_ELEMENT_COUNT];
    Label labels[MAX_MENU_ELEMENT_COUNT];
    Slider* sliders[MAX_MENU_ELEMENT_COUNT];
    int buttonCount, labelCount;

    Layout* layout;
    int selection;
} SubMenu;

typedef struct Menu {
    SubMenu subMenus[SUBMENU_COUNT];
    int currentSubMenu;

    SDL_Point oldMousePos;
    bool stateChanged;
} Menu;

Menu* initMenu(KeyStates* keystates);

ModeUpdateResult updateMenu(Menu* menu, KeyStates* keyStates, Graphics* gfx);

void destroyMenu(Menu* menu);

