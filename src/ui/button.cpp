#include "button.h"

Button* createButton(SDL_Rect rect, const char* name)
{
    Button* button = (Button*)malloc(sizeof(Button));

    strncpy(button->name, name, MAX_BUTTON_NAME_LENGTH);
    button->rect = rect;
    button->state = BUTTON_UNSELECTED;

    return button;
}

void destroyButton(Button* button)
{
    free(button);
}

