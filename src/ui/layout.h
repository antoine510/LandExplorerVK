#pragma once

#include "../common.h"
#include "utility/vector.h"

#define MAX_LAYOUT_ELEMENT_COUNT 256

typedef struct LayoutElement {
    SDL_Rect rect;
    int index;
    bool enabled : 1;
} LayoutElement;

typedef struct Layout {
    LayoutElement elements[MAX_LAYOUT_ELEMENT_COUNT];
    int elementCount;
} Layout;

Layout* layout_create();

void layout_addElement(Layout* layout, SDL_Rect rect, int index, Vec2 screenOrigin = Vec2{});
void layout_addExistingElement(Layout* layout, LayoutElement element);
void layout_enableElement(Layout* layout, int index, bool enable);
int layout_getElement(Layout* layout, int posX, int posY);

void layout_destroy(Layout* Layout);

