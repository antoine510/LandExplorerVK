#include "editorRenderer.h"
#include "graphics/graphics.h"
#include <cstring>

void initEditorRenderer(EditorRenderer* editorRenderer, Graphics* gfx) {
	editorRenderer->moduleTextTexture = NULL;
	editorRenderer->font = TTF_OpenFont("txt.ttf", 32);

	SDL_Color color = {255, 255, 0, 255};
	editorRenderer->modeNameTexture = new Sprite("Editor", color, editorRenderer->font);
	editorRenderer->modeNameTexture->setPosition(10, 10);

	editorRenderer->moduleTextTexture = new Sprite("Nom du module: ", color, editorRenderer->font);
	editorRenderer->moduleTextTexture->setOrigin(0.5f, 0.5f).setScreenOrigin(0.5f, 0.5f).setPosition(0, 0);

	editorRenderer->selectRefTex = new Sprite(1, 1);
	editorRenderer->selectRefTex->fillColor(SDL_Color{255, 0, 0, 128}).setScale(BLOC_SIZE);

	editorRenderer->selectArcherTex = new Sprite(1, 1);
	editorRenderer->selectArcherTex->fillColor(SDL_Color{255, 255, 0, 128}).setScale(BLOC_SIZE);

	editorRenderer->selectGuardTex = new Sprite(1, 1);
	editorRenderer->selectGuardTex->fillColor(SDL_Color{0, 255, 0, 128}).setScale(BLOC_SIZE);

	editorRenderer->selectModuleTex = new Sprite(1, 1);
	editorRenderer->selectModuleTex->fillColor(SDL_Color{0, 0, 255, 64});

	editorRenderer->bgOverlay = new Sprite(1, 1);
	editorRenderer->bgOverlay->fillColor(SDL_Color{0, 0, 0, 64}).setFullscreen();
}

void renderEditor(Graphics* gfx, Editor* editor) {
	if(editor->textChanged) {
		std::string instructions = "Nom du module : ";
		instructions += editor->moduleName;
		SDL_Color color = {255, 255, 255, 255};

		delete gfx->editorRenderer.moduleTextTexture;
		gfx->editorRenderer.moduleTextTexture = new Sprite(instructions, color, gfx->editorRenderer.font);
		gfx->editorRenderer.moduleTextTexture->setOrigin(0.5f, 0.5f).setScreenOrigin(0.5f, 0.5f).setPosition(0, 0);

		editor->textChanged = false;
	}

	switch(editor->mode) {
	case VIEWING: break;
	case SELECTING_MODULE:
		gfx->editorRenderer.selectModuleTex->setScale(float(editor->selection.w * BLOC_SIZE), float(editor->selection.h * BLOC_SIZE))
			.setPosition((editor->selection.x + (editor->selection.w > 0 ? 0 : 1)) * BLOC_SIZE - gfx->viewOrigin.x,
			(editor->selection.y + (editor->selection.h > 0 ? 0 : 1)) * BLOC_SIZE - gfx->viewOrigin.y);
		gfx->editorRenderer.selectModuleTex->draw(gfx->cmdBuf);
		break;
	case SELECTING_REFERENCE:
		gfx->editorRenderer.selectRefTex->setPosition(editor->module->ref.x * BLOC_SIZE - gfx->viewOrigin.x,
													  editor->module->ref.y * BLOC_SIZE - gfx->viewOrigin.y);
		gfx->editorRenderer.selectRefTex->draw(gfx->cmdBuf);
		break;
	case SELECTING_ARCHERS:
		gfx->editorRenderer.selectArcherTex->setPosition(editor->module->archerPos[editor->module->archerCount].x * BLOC_SIZE - gfx->viewOrigin.x,
														 editor->module->archerPos[editor->module->archerCount].y * BLOC_SIZE - gfx->viewOrigin.y);
		gfx->editorRenderer.selectArcherTex->draw(gfx->cmdBuf);
		break;
	case SELECTING_GUARDS:
		gfx->editorRenderer.selectGuardTex->setPosition(editor->module->guardPos[editor->module->guardCount].x * BLOC_SIZE - gfx->viewOrigin.x,
														editor->module->guardPos[editor->module->guardCount].y * BLOC_SIZE - gfx->viewOrigin.y);
		gfx->editorRenderer.selectGuardTex->draw(gfx->cmdBuf);
		break;
	case ENTERING_NAME:
		gfx->editorRenderer.bgOverlay->draw(gfx->cmdBuf);
		gfx->editorRenderer.moduleTextTexture->draw(gfx->cmdBuf);
		break;
	}

	gfx->editorRenderer.modeNameTexture->draw(gfx->cmdBuf);
}

void destroyEditorRenderer(EditorRenderer* editorRenderer) {
	TTF_CloseFont(editorRenderer->font);
	delete editorRenderer->moduleTextTexture;
	delete editorRenderer->modeNameTexture;
	delete editorRenderer->selectRefTex;
	delete editorRenderer->selectModuleTex;
	delete editorRenderer->selectArcherTex;
	delete editorRenderer->selectGuardTex;
	delete editorRenderer->bgOverlay;
}

