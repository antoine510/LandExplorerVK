#include "slider.h"
#include <algorithm>

Slider slider_create(std::string name, SDL_Point pos, int value, int min, int max, Vec2 screenOrigin)
{
	Slider slider;
    slider.name = std::move(name);
    slider.pos = pos;
	slider.screenOrigin = screenOrigin;
    slider.value = value;
    slider.max = max;
    slider.min = min;

    return slider;
}

void slider_changeValue(Slider& slider, int change)
{
    slider.value += change;
	slider.value = std::clamp(slider.value, slider.min, slider.max);
}

SDL_Rect slider_getMinusRect(const Slider& slider)
{
    return SDL_Rect{slider.pos.x + 300, slider.pos.y, 30, 30};
}

SDL_Rect slider_getPlusRect(const Slider& slider)
{
    return SDL_Rect{slider.pos.x + 450, slider.pos.y, 30, 30};
}
