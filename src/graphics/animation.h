#pragma once

#include "../common.h"
#include "../timer.h"

#define MAX_ANIMATION_FRAMES 16
#define MAX_ANIMATION_USER 8

typedef struct Animation
{
	int textureID;
    int frameCount;
    int delay;
    int timerIndices[MAX_ANIMATION_USER];

    Uint8 isLooping;
} Animation;

Animation* initAnimation(int textureID, int width, int frameWidth, int delay, Uint8 isLooping);

int getFrameClip(Animation* animation, int user);
void reset(Animation* animation, int user);

void destroyAnimation(Animation* animation);

