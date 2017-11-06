#pragma once

#include "common.h"

typedef enum ModeUpdateResult {
    MURESULT_QUIT = 0,
    MURESULT_MENU = 1,
    MURESULT_LEVEL = 2,
    MURESULT_MAP = 3,
    MURESULT_LEVEL_LOAD = 4,
    MURESULT_EDITOR = 5,
    MURESULT_WIN = 6
} ModeUpdateResult;

