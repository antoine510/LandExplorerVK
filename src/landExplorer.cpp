#include "landExplorer.h"
#include "timer.h"
#include "utility/random.h"

#include <SDL_ttf.h>
#include <SDL_image.h>

void initExplorer(LandExplorer* landExplorer)
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
        printf("Failed to initialize SDL : %s", SDL_GetError());
    }
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("Failed to initialize SDL_image : %s", IMG_GetError());
    }
    if(TTF_Init() == -1) {
        printf("Failed to initialize SDL_TTF: %s\n", TTF_GetError());
    }

    timerStack = initTimerStack();
    initKeystates(&landExplorer->keyStates);
    landExplorer->gfx = initGraphics();
    landExplorer->sound = sound_create();
    landExplorer->menu = initMenu(&landExplorer->keyStates);
    landExplorer->mapp = initMap();
    landExplorer->editor = initEditor(&landExplorer->gfx->viewOrigin);
    landExplorer->credits = createCredits();
    landExplorer->level = NULL;

    soundstack_changeMusic(MUSIC_TITLESCREEN);

    landExplorer->mode = MURESULT_MENU;
}

void mainLoop(LandExplorer* landExplorer)
{
    const int totTime = (1000 / FPS);
    int initTime;
    do
    {
		if(SDL_HasEvent(SDL_QUIT)) landExplorer->mode = MURESULT_QUIT;

        initTime = SDL_GetTicks();
		startFrame(landExplorer->gfx);
		spriteRenderer->bind(landExplorer->gfx->cmdBuf);

        updateTimers(timerStack);
        processKeystates(&landExplorer->keyStates);

		//TODO: enable sprite endering
        switch (landExplorer->mode)
        {
        case MURESULT_LEVEL:
            landExplorer->mode = updateLevel(landExplorer->level, &landExplorer->keyStates);
            renderLevel(landExplorer->gfx, landExplorer->level);
            updateMapRendering(&landExplorer->gfx->mapRenderer, landExplorer->gfx, landExplorer->mapp, landExplorer->level->terrain);
            break;
        case MURESULT_MENU:
            landExplorer->mode = updateMenu(landExplorer->menu, &landExplorer->keyStates, landExplorer->gfx);
            renderMenu(landExplorer->gfx, landExplorer->menu);
            break;
        case MURESULT_MAP:
            landExplorer->mode = updateMap(landExplorer->mapp, &landExplorer->keyStates);
            renderMap(&landExplorer->gfx->mapRenderer, landExplorer->gfx, landExplorer->mapp, landExplorer->level);
            break;
        case MURESULT_EDITOR:
            landExplorer->mode = updateEditor(landExplorer->editor, landExplorer->level, &landExplorer->keyStates);
            renderLevel(landExplorer->gfx, landExplorer->level);
            renderEditor(landExplorer->gfx, landExplorer->editor);
            break;
        case MURESULT_LEVEL_LOAD:
            renderBackground(landExplorer->gfx->bgRenderer, landExplorer->gfx, BG_CREATION); //Signal the user we are loading the level
			presentFrame(landExplorer->gfx);
			startFrame(landExplorer->gfx);
            landExplorer->level = initLevel(&landExplorer->gfx->viewOrigin);
			terrainRenderer->setTerrain(landExplorer->level->terrain);
            initCamera(landExplorer->gfx, landExplorer->level->entities);
            setBackgroundRendererTime(landExplorer->gfx->bgRenderer, &landExplorer->level->timeMinutes);
            initCreditsRenderer(landExplorer->credits, landExplorer->gfx);

            if(Random<int>::range(0, 1) == 0) soundstack_changeMusic(MUSIC_OVERWORLD); else soundstack_changeMusic(MUSIC_OVERWORLD_ALT);

            SDL_Delay(2000);

            landExplorer->mode = MURESULT_LEVEL;
            break;
        case MURESULT_WIN:
            updateCredits(landExplorer->credits);
            renderCredits(landExplorer->gfx, landExplorer->credits);
        default: break;
        }

		presentFrame(landExplorer->gfx);
        sound_playAllSounds(landExplorer->sound);

        //SDL_Delay(std::max(0, totTime - (int)SDL_GetTicks() + initTime));
    } while (!isKeyHeld(&landExplorer->keyStates, key_quit) && landExplorer->mode != MURESULT_QUIT);
}

void destroyExplorer(LandExplorer* landExplorer)
{
    if(landExplorer->level != NULL) destroyLevel(landExplorer->level);
    destroyCredits(landExplorer->credits);
    destroyEditor(landExplorer->editor);
    destroyMap(landExplorer->mapp);
    destroyMenu(landExplorer->menu);
    sound_destroy(landExplorer->sound);
    destroyGraphics(landExplorer->gfx);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
	LandExplorer landExplorer;

	initExplorer(&landExplorer);
	mainLoop(&landExplorer);
	destroyExplorer(&landExplorer);

	return 0;
}
