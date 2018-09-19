#pragma once

#include "common.h"

#define MAX_CREDIT_LINES 128

typedef struct Credits {
    std::string lines[MAX_CREDIT_LINES];
    int lineSize[MAX_CREDIT_LINES];
    int lineCount;

    std::string endLine;
    int endLineSize;

    float baseY;
} Credits;

Credits* createCredits();

void updateCredits(Credits* credits);

void destroyCredits(Credits* credits);
