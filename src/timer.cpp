#include "timer.h"

void initTimer(Timer* timer, unsigned int ticksDelay, Uint8 pausable, Uint8 isLooping)
{
    timer->ticksDelay = ticksDelay;
    timer->pausable = pausable;
    timer->isLooping = isLooping;
}

void startTimer(Timer* timer)
{
    if (!timer->running)
    {
        timer->startTime = SDL_GetTicks();
        timer->ellapsed = 0;
        timer->running = 1;
        timer->finished = 0;
        timer->paused = 0;
    }
}

void pauseTimer(Timer* timer, bool b)
{
    if(timer->running && timer->pausable)
    {
        if(b && !timer->paused)
        {
            timer->ellapsed = SDL_GetTicks() - timer->startTime;
            timer->paused = 1;
        }
        else if(!b && timer->paused)
        {
            timer->startTime = SDL_GetTicks() - timer->ellapsed;
            timer->paused = 0;
        }
    }
}

void stopTimer(Timer* timer)
{
    if(timer->running)
    {
        timer->running = 0;
        timer->ellapsed = 0;
    }
}

void restartTimer(Timer* timer)
{
    stopTimer(timer);
    startTimer(timer);
}

void updateTimer(Timer* timer, Uint32 time)
{
    if(!timer->paused)
    {
        timer->ellapsed = time - timer->startTime;
        if (!timer->isLooping && timer->ellapsed > timer->ticksDelay)
        {
            stopTimer(timer);
            timer->finished = 1;
        }
    }
}

int getLoopingCount(Timer* timer)
{
    return timer->ellapsed / timer->ticksDelay;
}


Timer* initTimerStack()
{
    Timer* timers = (Timer*)calloc(MAX_TIMER_COUNT, sizeof(Timer));
    return timers;
}

int getFreeTimer(Timer* timers)
{
    int i;
    for(i = 0; i < MAX_TIMER_COUNT; i++)
    {
        if(!timers[i].used)
        {
            timers[i].used = 1;
            return i;
        }
    }
    printf("Error : Not enough timer slots");
    return -1;
}

void getFreeTimers(Timer* timers, int* dst, int count)
{
    int i;
    for(i = 0; i < count; i++)
    {
        dst[i] = getFreeTimer(timers);
    }
}

void pauseTimers(Timer* timers, Uint8 b)
{
    int i;
    for(i = 0; i < MAX_TIMER_COUNT; i++)
    {
        if(timers[i].used) pauseTimer(&timers[i], b);
    }
}

void updateTimers(Timer* timers)
{
    int i;
    for(i = 0; i < MAX_TIMER_COUNT; i++)
    {
        if(timers[i].used)
        {
            updateTimer(&timers[i], SDL_GetTicks());
        }
    }
}

void destroyTimerStack(Timer* timers)
{
    free(timers);
}
