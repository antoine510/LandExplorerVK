#include "slider.h"
#include <algorithm>

Slider* slider_create(char* name, SDL_Point pos, int value, int min, int max)
{
    Slider* slider = (Slider*)malloc(sizeof(Slider));
    slider->name = name;
    slider->pos = pos;
    slider->value = value;
    slider->max = max;
    slider->min = min;

    return slider;
}

void slider_changeValue(Slider* slider, int change)
{
    slider->value += change;
	std::clamp(slider->value, slider->min, slider->max);
}

SDL_Rect slider_getMinusRect(Slider* slider)
{
    return SDL_Rect{slider->pos.x + 300, slider->pos.y, 30, 30};
}

SDL_Rect slider_getPlusRect(Slider* slider)
{
    return SDL_Rect{slider->pos.x + 450, slider->pos.y, 30, 30};
}

void slider_destroy(Slider* slider)
{
    free(slider->name);
    free(slider);
}
