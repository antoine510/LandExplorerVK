#include "button.h"

Button createButton(SDL_Rect rect, std::string name, Vec2 screenOrigin)
{
    Button button;

	button.name = std::move(name);
    button.rect = rect;
    button.state = BUTTON_UNSELECTED;
	button.screenOrigin = screenOrigin;

    return button;
}

