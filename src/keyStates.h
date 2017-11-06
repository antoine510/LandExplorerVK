#pragma once

#include "common.h"
#include "entities.h"

typedef enum KeyType {	key_quit = 0, key_validate = 1, key_left = 2, key_right = 3, key_up = 4, key_down = 5, key_jump = 6, key_select = 7,
						key_map = 8, key_editor = 9, key_inventory = 10, key_backspace = 11} KeyType;

typedef struct KeyStates {
    SDL_Scancode keys[256];
    bool lastKeyState[256];
    const Uint8* states;
} KeyStates;

void initKeystates(KeyStates* keystates);

void processKeystates(KeyStates* keyStates);

bool isKeyPressed(KeyStates* keystates, KeyType type);
bool isKeyHeld(KeyStates* keystates, KeyType type);

void keystates_setKey(KeyStates* keystates, KeyType type);
std::string keystates_getKeyName(KeyStates* keystates, KeyType type);

Uint8 isMouseLeftDown();
Uint8 isMouseLeftUp();
Uint8 isMouseRightDown();
Uint8 isMouseRightUp();
bool isHoldMouseLeft();
bool isHoldMouseRight();

Uint8 textInput(char* text);
void mouseWheelIncrement(int* value);

