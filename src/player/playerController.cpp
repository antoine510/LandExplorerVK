#include "playerController.h"
#include "../terrain/terrainGeneration.h"
#include "../terrain/blockTypes.h"
#include "../graphics/displayInfo.h"
#include "../sound.h"
#include "../level.h"
#include "../timer.h"
#include "../physics/collisions.h"
#include <algorithm>

static void activateInventory(PlayerControl* pControl, bool activate);
static void playerRespawn(Level* level, int playerID);

PlayerControl* initPlayerControl(SDL_Point* viewOrigin, Entities* entities, int playerID)
{
    PlayerControl* pControl = (PlayerControl*)malloc(sizeof(PlayerControl));
    pControl->inventory = initInventory();
    pControl->inventoryActive = false;
    pControl->cursorItemID = 0;
    pControl->cursorItemCount = 0;
    pControl->viewOrigin = viewOrigin;
    pControl->playerLife = &entities->status[playerID].HP;
    pControl->currentWeaponID = -1;
    pControl->breakTimer = 0;
    pControl->breaking = 0;
    pControl->breakingX = 0; pControl->breakingY = 0;
    pControl->isUsing = false;
    pControl->usingTimerID = getFreeTimer(timerStack);
    initTimer(&timerStack[pControl->usingTimerID], 1000, true, false);
    pControl->deathTimerID = getFreeTimer(timerStack);
    initTimer(&timerStack[pControl->deathTimerID], PLAYER_DEATH_LENGTH, true, false);
    pControl->isDead = false;

    pControl->inventoryLayout = layout_create();
    SDL_Rect itemRect = {20, 20, 32, 32};
    int x, y;
    for(y = 0; y < INVENTORY_HEIGHT; y++)
    {
        for(x = 0; x < INVENTORY_WIDTH; x++)
        {
            layout_addElement(pControl->inventoryLayout, itemRect, getSlotIndex(x, y));
            itemRect.x += 40;
        }
        itemRect.x = 20;
        itemRect.y += 40;
    }

    return pControl;
}

void updateController(KeyStates* keyStates, Level* level, int playerID, PlayerControl* pControl)
{
    int x, y;
    SDL_GetMouseState(&x, &y);
	x -= myDisplayMode.w / 2;
	y = myDisplayMode.h / 2 - y;

    if(pControl->currentWeaponID == -1)
        pControl->direction = (x >= 0) ? 1 : -1;

    if(isKeyHeld(keyStates, key_inventory))
    {
        activateInventory(pControl, !pControl->inventoryActive);
    }
    else if(isMouseLeftDown())
    {
        int elt = layout_getElement(pControl->inventoryLayout, x, y);
        if(elt != -1)
        {
            exchangeItem(pControl->inventory, &pControl->cursorItemID, &pControl->cursorItemCount, elt);
        }
    }


    //Death handling
    if(pControl->isDead)
    {
        if(!timerStack[pControl->deathTimerID].running)
        {
            playerRespawn(level, playerID);
            pControl->isDead = false;
        }
        return;
    }
    else if(*pControl->playerLife <= 0.0f)
    {
        startTimer(&timerStack[pControl->deathTimerID]);
        pControl->isDead = true;
        level->entities->gfxData[playerID].texState = 1;
        level->entities->dynData[playerID].moving = 0;
        level->entities->status[playerID].HP = 0.0f;
        return;
    }

    //Block destruction and items usage
    if(isMouseRightDown())
    {
        pControl->isUsing = true;
        initTimer(&timerStack[pControl->usingTimerID], getCurrentItem(pControl->inventory).usageTime, true, false);
        startTimer(&timerStack[pControl->usingTimerID]);
    }
    else if(isMouseRightUp()) {pControl->isUsing = false;}

    updateSelection(pControl->inventory);

    if(pControl->isUsing && !timerStack[pControl->usingTimerID].running)
    {
        int posx = (pControl->viewOrigin->x + x) / BLOC_SIZE, posy = (pControl->viewOrigin->y - y) / BLOC_SIZE;
        useCurrentItem(pControl->inventory, level, posx, posy);
        startTimer(&timerStack[pControl->usingTimerID]);
    }
    else if(isHoldMouseLeft())
    {
        Uint32 posx = (pControl->viewOrigin->x + x) / BLOC_SIZE, posy = (pControl->viewOrigin->y - y) / BLOC_SIZE;
        if(checkBreakable(level->terrain->blocTypes, getBlock(level->terrain, posx, posy)) || (getBackwall(level->terrain, posx, posy).type != 0))
        {
            if(posx != pControl->breakingX || posy != pControl->breakingY)
            {
                pControl->breaking = 1;
                pControl->breakingX = posx; pControl->breakingY = posy;
                pControl->breakTimer = (int)BLOC_BREAKING_TIME;
            }
            else if(pControl->breakTimer == 0)
            {
                if(pControl->breaking >= BLOC_BREAKING_STEPS-1)
                {
                    breakBloc(level->terrain, level->waterManager, posx, posy);
                    pControl->breaking = 1;
                }
                else
                {
                    pControl->breaking++;
                }
                pControl->breakTimer = (int)BLOC_BREAKING_TIME;
            }
            else
            {
                pControl->breakTimer--;
            }
        }
        else{pControl->breaking = 0; pControl->breakTimer = 0;}
    }
    else {pControl->breaking = 0; pControl->breakTimer = 0;}

    static int frame = 0;
    if(frame % (int)PCPS == 0)
    {
        level->entities->status[playerID].HP += HEALTH_REGEN_PER_SECOND;
        std::clamp(level->entities->status[playerID].HP, 0.0f, MAX_PLAYER_HP);
    }
    frame++;


    //Player movement and health control
    level->entities->gfxData[playerID].texState = 0;
    level->entities->dynData[playerID].moving = 0;

    float* velx = &level->entities->dynData[playerID].velocity.x;
    if(isKeyPressed(keyStates, key_right))
    {
        if(*velx < MAX_SIDEWARD_VELOCITY) *velx += SIDEWARD_ACCELERATION / PCPS;
        level->entities->dynData[playerID].moving = 1;
        level->entities->gfxData[playerID].texState = 100;
    }
    else if(isKeyPressed(keyStates, key_left))
    {
        if(*velx > -MAX_SIDEWARD_VELOCITY) *velx -= SIDEWARD_ACCELERATION / PCPS;
        level->entities->dynData[playerID].moving = 1;
        level->entities->gfxData[playerID].texState = 100;
    }

    if(level->entities->dynData[playerID].touchGround)
    {
        if(isKeyPressed(keyStates, key_jump))
        {
            level->entities->dynData[playerID].velocity.y -= JUMP_VELOCITY;
            level->entities->dynData[playerID].moving = 1;
        }
    }
    else if(level->entities->dynData[playerID].velocity.y > 15.0)
    {
        level->entities->gfxData[playerID].texState = 100 + ((level->entities->dynData[playerID].velocity.x < 0) ? 1 : 2);
    }
}

void playerRespawn(Level* level, int playerID)
{
    level->entities->status[playerID].HP = MAX_PLAYER_HP;
    level->entities->collData[playerID].pos = level->spawnPoint;
    level->entities->dynData[playerID].velocity = Vec2{0.0f, 0.0f};
    level->entities->collData[playerID].collType = COLLTYPE_PLAYER;
}

void activateInventory(PlayerControl* pControl, bool activate)
{
    pControl->inventoryActive = activate;
    int x, y;
    for(y = 1; y < INVENTORY_HEIGHT; y++)
    {
        for(x = 0; x < INVENTORY_WIDTH; x++)
        {
            layout_enableElement(pControl->inventoryLayout, getSlotIndex(x, y), activate);
        }
    }

    if(activate) soundstack_addSound(SOUND_MENU_OPEN); else soundstack_addSound(SOUND_MENU_CLOSE);
}

void destroyPlayerControl(PlayerControl* pControl)
{
    destroyInventory(pControl->inventory);
    free(pControl);
}
