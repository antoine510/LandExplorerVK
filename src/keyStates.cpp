#include "keyStates.h"
#include "components.h"
#include "sound.h"
#include <cstring>

static Uint8 isMouse(Uint32 type, Uint8 subType);

void initKeystates(KeyStates* keystates)
{
    keystates->keys[key_quit] = SDL_SCANCODE_ESCAPE;
    keystates->keys[key_validate] = SDL_SCANCODE_RETURN;
    keystates->keys[key_left] = SDL_SCANCODE_A;
    keystates->keys[key_right] = SDL_SCANCODE_D;
    keystates->keys[key_up] = SDL_SCANCODE_UP;
    keystates->keys[key_down] = SDL_SCANCODE_DOWN;
    keystates->keys[key_jump] = SDL_SCANCODE_SPACE;
    keystates->keys[key_select] = SDL_SCANCODE_SPACE;
    keystates->keys[key_map] = SDL_SCANCODE_SEMICOLON;
    keystates->keys[key_editor] = SDL_SCANCODE_E;
    keystates->keys[key_inventory] = SDL_SCANCODE_Q;
    keystates->keys[key_backspace] = SDL_SCANCODE_BACKSPACE;
    memset(keystates->lastKeyState, 0, 256*sizeof(bool));
}

void processKeystates(KeyStates* keyStates)
{
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
    SDL_PumpEvents();
    keyStates->states = SDL_GetKeyboardState(NULL);
}

bool isKeyPressed(KeyStates* keystates, KeyType type)
{
    return keystates->states[keystates->keys[type]];
}

bool isKeyHeld(KeyStates* keystates, KeyType type)
{
    bool p = keystates->states[keystates->keys[type]];
    bool h = (p && !keystates->lastKeyState[type]);
    keystates->lastKeyState[type] = p;
    return h;
}

void keystates_setKey(KeyStates* keystates, KeyType type)
{
    SDL_Event event;
    do {
        SDL_WaitEvent(&event);
    } while(event.type != SDL_KEYDOWN || event.key.keysym.scancode == keystates->keys[key_select]);
    keystates->keys[type] = event.key.keysym.scancode;
    keystates->lastKeyState[type] = true;
    soundstack_addSound(SOUND_MENU_OPEN);
}

std::string keystates_getKeyName(KeyStates* keystates, KeyType type)
{
    return SDL_GetKeyName(SDL_GetKeyFromScancode(keystates->keys[type]));
}

Uint8 isMouseLeftDown() {return isMouse(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_LEFT);}
Uint8 isMouseLeftUp() {return isMouse(SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT);}
Uint8 isMouseRightDown() {return isMouse(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_RIGHT);}
Uint8 isMouseRightUp() {return isMouse(SDL_MOUSEBUTTONUP, SDL_BUTTON_RIGHT);}
bool isHoldMouseLeft() {return SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(SDL_BUTTON_LEFT);}
bool isHoldMouseRight() {return SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(SDL_BUTTON_RIGHT);}

Uint8 isMouse(Uint32 type, Uint8 subType)
{
    SDL_Event event;
    Uint8 mouseActive = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, type, type) != 0;
    return mouseActive && (event.button.button == subType);
}

Uint8 textInput(char* text)
{
    SDL_Event event;
    Uint8 textActive = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_TEXTINPUT, SDL_TEXTINPUT) != 0;
    if(textActive) strcat(text, event.text.text);
    return textActive;
}

void mouseWheelIncrement(int* value)
{
    SDL_Event event;
    Uint8 wheelMoved = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_MOUSEWHEEL, SDL_MOUSEWHEEL) != 0;
    if(wheelMoved) (*value) += event.wheel.y;
}
