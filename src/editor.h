#pragma once

#include "common.h"
#include "modeUpdateResult.h"
#include "level.h"
#include "keyStates.h"
#include "terrain/module.h"

#define MAX_MODULE_NAME_LENGTH 64

typedef enum EditorMode {VIEWING, SELECTING_MODULE, SELECTING_REFERENCE, SELECTING_ARCHERS, SELECTING_GUARDS, ENTERING_NAME} EditorMode;

typedef struct Editor {
    Vec4* viewOrigin;

    EditorMode mode;
    Module* module;
    SDL_Rect selection;

    Uint8 textChanged;
    char moduleName[MAX_MODULE_NAME_LENGTH];
} Editor;

Editor* initEditor(Vec4* viewOrigin);

ModeUpdateResult updateEditor(Editor* editor, Level* level, KeyStates* keyStates);

void destroyEditor(Editor* editor);

