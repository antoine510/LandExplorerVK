#include "animation.h"

Animation* initAnimation(int textureID, int width, int frameWidth, int delay, Uint8 isLooping)
{
    Animation* animation = (Animation*)malloc(sizeof(Animation));
    int frameCount = width / frameWidth;

    animation->textureID = textureID;
    animation->frameCount = frameCount;
    animation->delay = delay;
    animation->isLooping = isLooping;
    getFreeTimers(timerStack, animation->timerIndices, MAX_ANIMATION_USER);

    int i;
    for(i = 0; i < MAX_ANIMATION_USER; i++)
    {
        initTimer(&timerStack[animation->timerIndices[i]], delay, 1, isLooping);
    }

    return animation;
}

int getFrameClip(Animation* animation, int user)
{
    startTimer(&timerStack[animation->timerIndices[user]]);
    int curFrame = getLoopingCount(&timerStack[animation->timerIndices[user]]);

    if(curFrame >= animation->frameCount)
    {
        if(!animation->isLooping)
            curFrame = animation->frameCount-1;
        else
            curFrame %= animation->frameCount;
    }

    return curFrame;
}

void reset(Animation* animation, int user)
{
    stopTimer(&timerStack[animation->timerIndices[user]]);
}

void destroyAnimation(Animation* animation)
{
    int i;
    for(i = 0; i < MAX_ANIMATION_USER; i++) timerStack[animation->timerIndices[i]].used = 0;
    free(animation);
}
