#include "editorRenderer.h"
#include "../graphics.h"
#include <cstring>

void initEditorRenderer(EditorRenderer* editorRenderer, Graphics* gfx)
{
    editorRenderer->moduleTextTexture = NULL;
    editorRenderer->font = TTF_OpenFont("txt.ttf", 32);

    SDL_Color color = {255, 255, 0, 255};
    SDL_Surface* t = TTF_RenderText_Blended(editorRenderer->font, "Editor", color);
    editorRenderer->modeNameTexture = createTextureFromSurface(gfx->renderer, t);
	setTextureOriginRatio(editorRenderer->modeNameTexture, 0.0f, 1.0f);
	setTexturePos(editorRenderer->modeNameTexture, -myDisplayMode.w/2 + 10, myDisplayMode.h/2 - 10);
    SDL_FreeSurface(t);

    t = TTF_RenderText_Blended(editorRenderer->font, "Nom du module : ", color);
    editorRenderer->moduleTextTexture = createTextureFromSurface(gfx->renderer, t);
	setTextureOriginRatio(editorRenderer->moduleTextTexture, 0.5f, 0.5f);
    setTexturePos(editorRenderer->moduleTextTexture, 0, 0);
    SDL_FreeSurface(t);

    t = SDL_CreateRGBSurface(0, 1, 1, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

    SDL_FillRect(t, NULL, SDL_MapRGBA(t->format, 255, 0, 0, 128));
    editorRenderer->selectRefTex = createTextureFromSurface(gfx->renderer, t);
    setTextureScale(editorRenderer->selectRefTex, BLOC_SIZE);

    SDL_FillRect(t, NULL, SDL_MapRGBA(t->format, 255, 255, 0, 128));
    editorRenderer->selectArcherTex = createTextureFromSurface(gfx->renderer, t);
    setTextureScale(editorRenderer->selectArcherTex, BLOC_SIZE);

    SDL_FillRect(t, NULL, SDL_MapRGBA(t->format, 0, 255, 0, 128));
    editorRenderer->selectGuardTex = createTextureFromSurface(gfx->renderer, t);
    setTextureScale(editorRenderer->selectGuardTex, BLOC_SIZE);

    SDL_FreeSurface(t);

    t = SDL_CreateRGBSurface(0, 1, 1, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_FillRect(t, NULL, SDL_MapRGBA(t->format, 0, 0, 255, 64));
    editorRenderer->selectModuleTex = createTextureFromSurface(gfx->renderer, t);
    SDL_FreeSurface(t);

    t = SDL_CreateRGBSurface(0, 1, 1, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_FillRect(t, NULL, SDL_MapRGBA(t->format, 0, 0, 0, 64));
    editorRenderer->bgOverlay = createTextureFromSurface(gfx->renderer, t);
    setTextureFullscreen(editorRenderer->bgOverlay);
    SDL_FreeSurface(t);
}

void renderEditor(Graphics* gfx, Editor* editor)
{
    if(editor->textChanged)
    {
        char instructions[200] = "Nom du module : ";
        strcat(instructions, editor->moduleName);
        SDL_Color color = {255, 255, 255, 255};

        SDL_Surface* t = TTF_RenderText_Blended(gfx->editorRenderer.font, instructions, color);
        modifyTextureFromSurface(gfx->editorRenderer.moduleTextTexture, t);
		setTextureOriginRatio(gfx->editorRenderer.moduleTextTexture, 0.5f, 0.5f);
        setTexturePos(gfx->editorRenderer.moduleTextTexture, 0, 0);
        SDL_FreeSurface(t);

        editor->textChanged = false;
    }

    startFrameTexturesRendering(gfx->renderer);
    switch (editor->mode)
    {
    case VIEWING: break;
    case SELECTING_MODULE:
        setTextureSize(gfx->editorRenderer.selectModuleTex, editor->selection.w * BLOC_SIZE, editor->selection.h * BLOC_SIZE);
        setTexturePos(gfx->editorRenderer.selectModuleTex,
                      (editor->selection.x + (editor->selection.w>0 ? 0 : 1)) * BLOC_SIZE - gfx->viewOrigin.x,
                      (editor->selection.y + (editor->selection.h>0 ? 0 : 1)) * BLOC_SIZE - gfx->viewOrigin.y);
        drawTexture(gfx->editorRenderer.selectModuleTex);
        break;
    case SELECTING_REFERENCE:
        setTexturePos(gfx->editorRenderer.selectRefTex,
                      editor->module->ref.x * BLOC_SIZE - gfx->viewOrigin.x,
                      editor->module->ref.y * BLOC_SIZE - gfx->viewOrigin.y);
        drawTexture(gfx->editorRenderer.selectRefTex);
        break;
    case SELECTING_ARCHERS:
        setTexturePos(gfx->editorRenderer.selectArcherTex,
                      editor->module->archerPos[editor->module->archerCount].x * BLOC_SIZE - gfx->viewOrigin.x,
                      editor->module->archerPos[editor->module->archerCount].y * BLOC_SIZE - gfx->viewOrigin.y);
        drawTexture(gfx->editorRenderer.selectArcherTex);
        break;
    case SELECTING_GUARDS:
        setTexturePos(gfx->editorRenderer.selectGuardTex,
                      editor->module->guardPos[editor->module->guardCount].x * BLOC_SIZE - gfx->viewOrigin.x,
                      editor->module->guardPos[editor->module->guardCount].y * BLOC_SIZE - gfx->viewOrigin.y);
        drawTexture(gfx->editorRenderer.selectGuardTex);
        break;
    case ENTERING_NAME:
        drawTexture(gfx->editorRenderer.bgOverlay);
        drawTexture(gfx->editorRenderer.moduleTextTexture);
        break;
    }

    drawTexture(gfx->editorRenderer.modeNameTexture);

    endFrameTexturesRendering();
}

void destroyEditorRenderer(EditorRenderer* editorRenderer)
{
    TTF_CloseFont(editorRenderer->font);
    if(editorRenderer->moduleTextTexture != NULL) destroyTexture(editorRenderer->moduleTextTexture);
    destroyTexture(editorRenderer->modeNameTexture);
    destroyTexture(editorRenderer->selectRefTex);
    destroyTexture(editorRenderer->selectModuleTex);
    destroyTexture(editorRenderer->bgOverlay);
}

