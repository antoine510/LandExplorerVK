#pragma once

#include "common.h"

#define MAX_TIMER_COUNT 64

typedef struct Timer
{
    unsigned int startTime;
    unsigned int ellapsed;
    unsigned int ticksDelay;

    Uint8 running;
    Uint8 finished;
    Uint8 paused;
    Uint8 pausable;
    Uint8 isLooping;

    Uint8 used;
} Timer;

void initTimer(Timer* timer, unsigned int ticksDelay, Uint8 pausable, Uint8 isLooping);

void startTimer(Timer* timer);
void pauseTimer(Timer* timer, bool b);
void stopTimer(Timer* timer);
void restartTimer(Timer* timer);
void updateTimer(Timer* timer, Uint32 time);
int getLoopingCount(Timer* timer);


Timer* initTimerStack();

int getFreeTimer(Timer* timers);
void getFreeTimers(Timer* timers, int* dst, int count);

void pauseTimers(Timer* timers, Uint8 b);
void updateTimers(Timer* timers);

void destroyTimerStack(Timer* timers);

extern Timer* timerStack;

