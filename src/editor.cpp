#include "editor.h"
#include "utility/mathUtility.h"

Editor* initEditor(SDL_Point* viewOrigin) {
	Editor* editor = (Editor*)calloc(1, sizeof(Editor));

	editor->viewOrigin = viewOrigin;
	editor->mode = VIEWING;
	editor->textChanged = true;

	return editor;
}

ModeUpdateResult updateEditor(Editor* editor, Level* level, KeyStates* keyStates) {
	int x, y;
	SDL_GetMouseState(&x, &y);
	int posx = (editor->viewOrigin->x + x) / BLOC_SIZE, posy = (editor->viewOrigin->y + y) / BLOC_SIZE;

	switch(editor->mode) {
	case VIEWING:
		if(isMouseLeftDown()) {
			editor->mode = SELECTING_MODULE;
			editor->selection.x = posx;
			editor->selection.y = posy;
		} else if(isMouseRightUp()) {
			Module* module = loadModule("puit");
			pasteModule(module, level, SDL_Point{posx, posy}, true, false);
			printf("Pasting module\n");
			destroyModule(module);
		} else if(isKeyHeld(keyStates, key_editor)) { return MURESULT_LEVEL; }
		break;
	case SELECTING_MODULE:
		editor->selection.w = addAbs(posx - editor->selection.x, 1);
		editor->selection.h = addAbs(posy - editor->selection.y, 1);

		if(isMouseLeftUp()) {
			editor->mode = SELECTING_REFERENCE;
			if(posx < editor->selection.x) std::swap(posx, editor->selection.x);   //If the rectangle is not legal we exchange corners
			if(posy < editor->selection.y) std::swap(posy, editor->selection.y);
			editor->selection.w = posx - editor->selection.x + 1;
			editor->selection.h = posy - editor->selection.y + 1;

			editor->module = getModuleFromTerrain(level->terrain, editor->selection);
		}
		break;
	case SELECTING_REFERENCE:
		editor->module->ref.x = posx; editor->module->ref.y = posy;

		if(isMouseLeftDown()) {
			editor->mode = SELECTING_ARCHERS;

			editor->module->ref.x -= editor->selection.x;
			editor->module->ref.y -= editor->selection.y;
		}
		break;
	case SELECTING_ARCHERS:
		if(isKeyHeld(keyStates, key_validate)) {
			editor->mode = SELECTING_GUARDS;
		} else if(isMouseLeftDown()) {
			editor->module->archerPos[editor->module->archerCount].x -= editor->selection.x;
			editor->module->archerPos[editor->module->archerCount].y -= editor->selection.y;
			editor->module->archerCount++;
		}
		editor->module->archerPos[editor->module->archerCount] = SDL_Point{posx, posy};     //The next archer
		break;
	case SELECTING_GUARDS:
		if(isKeyHeld(keyStates, key_validate)) {
			editor->mode = ENTERING_NAME;
			SDL_StartTextInput();
		} else if(isMouseLeftDown()) {
			editor->module->guardPos[editor->module->guardCount].x -= editor->selection.x;
			editor->module->guardPos[editor->module->guardCount].y -= editor->selection.y;
			editor->module->guardCount++;
		}
		editor->module->guardPos[editor->module->guardCount] = SDL_Point{posx, posy};         //The next guard
		break;
	case ENTERING_NAME:
		editor->textChanged = textInput(editor->moduleName);
		if(isKeyHeld(keyStates, key_validate))     //We want to validate our moduleName and save
		{
			editor->mode = VIEWING;
			SDL_StopTextInput();

			saveModule(editor->module, editor->moduleName);
			destroyModule(editor->module);
			editor->module = NULL;

			memset(editor->moduleName, '\0', MAX_MODULE_NAME_LENGTH * sizeof(char));
		} else if(isKeyHeld(keyStates, key_backspace))     //We want to erase one char in our module name
		{
			if(strlen(editor->moduleName) > 0u) {
				editor->moduleName[strlen(editor->moduleName) - 1] = '\0';
				editor->textChanged = 1;
			}
		}
		break;
	}

	return MURESULT_EDITOR;
}

void destroyEditor(Editor* editor) {
	free(editor);
}

