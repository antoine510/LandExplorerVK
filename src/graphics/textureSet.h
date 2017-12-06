#pragma once

#include "common.h"
#include "animation.h"
#include "sprite.h"
#include "utility/xmlTools.h"

#define MAX_TEXTURE_COUNT 5
#define MAX_ANIMATION_COUNT 5

#define ANIMATION_OFFSET 100

typedef struct TextureSet {
    Sprite* textures[MAX_TEXTURE_COUNT];
    Animation* animations[MAX_ANIMATION_COUNT];
    Sprite* mapIcon;

    Uint8 animated;
    int curAnim[MAX_ANIMATION_USER];
    int curUser;
} TextureSet;

TextureSet* createTextureSet(xmlNodePtr texSetNode, const char* path);

void setTexture(TextureSet* texSet, unsigned int textureID, Sprite* tex);
void setAnimation(TextureSet* texSet, Animation* animation, unsigned int id);
void setMapIcon(TextureSet* texSet, Sprite* mapIcon);
void setTexSetColorMod(TextureSet* texSet, SDL_Color color);

Sprite* getTexture(TextureSet* texSet, int textureID);
void texSetFrameReset(TextureSet* texSet);

void destroyTextureSet(TextureSet* texSet);

