#include "playerInterfaceRenderer.h"
#include "../graphics.h"

PlayerInterfaceRenderer* createPlayerInterfaceRenderer(Graphics* gfx) {
	PlayerInterfaceRenderer* playerInterfaceRenderer = (PlayerInterfaceRenderer*)calloc(1, sizeof(PlayerInterfaceRenderer));

	SDL_Color color = {255, 255, 255, 255};
	TTF_Font* font = TTF_OpenFont("txt.ttf", 70);
	playerInterfaceRenderer->deathTextTexture = new Sprite("You died !", color, font);
	playerInterfaceRenderer->deathTextTexture->setOrigin(0.5f, 0.5f).setScreenOrigin(0.5f, 0.5f).setPosition(0, 50);
	TTF_CloseFont(font);

	return playerInterfaceRenderer;
}

void renderPlayerInterface(Graphics* gfx, PlayerControl* pControl) {
	if(pControl->breaking)
		blitBreak(gfx->texPack, gfx->cmdBuf, pControl->breaking, Uint32((pControl->breakingX - gfx->viewOrigin.x) * BLOC_SIZE), Uint32((pControl->breakingY - gfx->viewOrigin.y) * BLOC_SIZE));
	int life = (int)(*pControl->playerLife);
	SDL_Point pos = {-140, 15};
	while(life > HEALTH_PER_HEARTH) {
		blitHearth(gfx->texPack, gfx->cmdBuf, pos, 1.0f);
		pos.x += 24;
		life -= HEALTH_PER_HEARTH;
	}
	blitHearth(gfx->texPack, gfx->cmdBuf, pos, (float)life / HEALTH_PER_HEARTH);

	SDL_Point slotPos = {20, 20};
	int x, y, maxy = pControl->inventoryActive ? INVENTORY_HEIGHT : 1;
	for(y = 0; y < maxy; y++) {
		for(x = 0; x < INVENTORY_WIDTH; x++) {
			if(getSlotIndex(x, y) == pControl->inventory->selection) {
				slotPos.x -= 4; slotPos.y -= 4;
				blitSlot(gfx->texPack, gfx->cmdBuf, slotPos, 1.25f);
				slotPos.x += 4; slotPos.y += 4;
			} else {
				blitSlot(gfx->texPack, gfx->cmdBuf, slotPos, 1.0f);
			}
			if(pControl->inventory->itemCount[getSlotIndex(x, y)] > 0)
				blitItem(gfx->texPack, gfx->cmdBuf, slotPos, pControl->inventory->itemID[getSlotIndex(x, y)]);
			slotPos.x += 40;
		}
		slotPos.x = 20;
		slotPos.y += 40;
	}
	if(pControl->cursorItemCount > 0) {
		int x, y;
		SDL_GetMouseState(&x, &y);
		blitItem(gfx->texPack, gfx->cmdBuf, SDL_Point{x, y}, pControl->cursorItemID);
	}
	if(pControl->isDead) gfx->playerInterfaceRenderer->deathTextTexture->draw(gfx->cmdBuf);
}

void destroyPlayerInterfaceRenderer(PlayerInterfaceRenderer* playerInterfaceRenderer) {
	delete playerInterfaceRenderer->deathTextTexture;
	free(playerInterfaceRenderer);
}

