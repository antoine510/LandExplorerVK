#include "layout.h"
#include "graphics/displayInfo.h"

Layout* layout_create()
{
    Layout* layout = (Layout*)malloc(sizeof(Layout));
    layout->elementCount = 0;

    return layout;
}

void layout_addElement(Layout* layout, SDL_Rect rect, int index, Vec2 screenOrigin)
{
	rect.x += int(screenOrigin.x * myDisplayMode.w); rect.y += int(screenOrigin.y * myDisplayMode.h);
    layout->elements[layout->elementCount] = LayoutElement{rect, index, true};
    layout->elementCount++;
}

void layout_addExistingElement(Layout* layout, LayoutElement element)
{
    layout->elements[layout->elementCount] = element;
    layout->elementCount++;
}

void layout_enableElement(Layout* layout, int index, bool enable)
{
    int i;
    for(i = 0; i < layout->elementCount; i++)
    {
        if(layout->elements[i].index == index)
        {
            layout->elements[i].enabled = enable;
            return;
        }
    }
}

int layout_getElement(Layout* layout, int posX, int posY)
{
    SDL_Rect pos = {posX, posY, 1, 1};
    int i;
    for(i = 0; i < layout->elementCount; i++)
    {
        if(layout->elements[i].enabled && SDL_HasIntersection(&layout->elements[i].rect, &pos) == SDL_TRUE)
            return i;
    }
    return -1;
}

void layout_destroy(Layout* layout)
{
    free(layout);
}

