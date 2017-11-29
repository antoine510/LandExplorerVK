#include "water.h"
#include "blockTypes.h"
#include "../graphics/chunkRenderer.h"

#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a > b ? a : b)

#define WATER_LEVEL_COUNT 8

#define UP 0x01
#define DOWN 0x02
#define RIGHT 0x04
#define LEFT 0x08

static int activeChunckComparator(const void* a, const void* b);
static int processContinuousActiveChuncks(ChunckCoords* activeChuncks, int activeChunckCount, Uint32 activeChunckID);

static void addActiveChunck(WaterManager* waterman, ChunckCoords coords);
static void deleteActiveChunck(WaterManager* waterman, Uint32 activeChunckID);

static void processWaterBloc(WaterManager* waterman, Terrain* terrain, Uint32 x, Uint32 y);
static void setLiquidLevel(Terrain* terrain, Uint32 x, Uint32 y, Uint8 level);
static Uint8 getLiquidLevel(Terrain* terrain, Uint32 x, Uint32 y);
static int getExtendedLiquidLevel(Terrain* terrain, Uint32 x, Uint32 y);
static bool checkTranferable(Uint8 type, Uint8 liquidLevel);

WaterManager* waterman_create()
{
    WaterManager* waterman = (WaterManager*)calloc(1, sizeof(WaterManager));

    return waterman;
}

void waterman_update(WaterManager* waterman, Terrain* terrain)
{
    int chunckGroupStart = 0, chunckGroupCount = 0, t = 0;

    while(chunckGroupStart < waterman->activeChunckCount)
    {
        chunckGroupCount = processContinuousActiveChuncks(waterman->activeChuncks, waterman->activeChunckCount, chunckGroupStart);
        bool cont = false;

        Uint32 xb = waterman->activeChuncks[chunckGroupStart].x * CHUNCK_WIDTH;
        Uint32 yb = waterman->activeChuncks[chunckGroupStart].y * CHUNCK_HEIGHT;
        Uint32 x, y;

        for(y = yb + CHUNCK_HEIGHT-1; y >= yb; y--)
        {
            for(x = xb; x < xb + chunckGroupCount * CHUNCK_WIDTH; x++)
            {
                if(checkLiquid(terrain->blocTypes, getBlock(terrain, x, y)))
                {
                    processWaterBloc(waterman, terrain, x, y);
                }
            }
            for(x = xb + chunckGroupCount * CHUNCK_WIDTH -1; x >= xb; x--)
            {
                if(checkLiquid(terrain->blocTypes, getBlock(terrain, x, y)))
                {
                    processWaterBloc(waterman, terrain, x, y);
                }
            }

            int lineWaterLevel = 0; bool waterOnLine = false;
            Bloc previous = getBlock(terrain, xb-1, y), current = getBlock(terrain, xb, y), next = getBlock(terrain, xb+1, y);
            for(x = xb; x < xb + chunckGroupCount * CHUNCK_WIDTH; x++)
            {
                if(checkLiquid(terrain->blocTypes, current))
                {
                    if(!waterOnLine)
                    {
                        lineWaterLevel = getExtendedLiquidLevel(terrain, x, y);
                        waterOnLine = true;
                    }
                    if(((previous.type == BLOC_NONE || next.type == BLOC_NONE) && lineWaterLevel > 1)
                                                  || abs(lineWaterLevel - getExtendedLiquidLevel(terrain, x, y)) > 1)
                    {cont = true;}
                }
                else if(checkSolid(terrain->blocTypes, current))
                {
                    waterOnLine = false;
                }
                previous = current; current = next; next = getBlock(terrain, x+2, y);
            }
        }

        if(!cont)
        {
            int i;
            for(i = 0; i < chunckGroupCount; i++)
            {
                deleteActiveChunck(waterman, chunckGroupStart + i);
            }
        }

        SDL_Rect chunckUpdateRect = {xb-1, yb-1, chunckGroupCount * CHUNCK_WIDTH+2, CHUNCK_HEIGHT+2};
        updateRect(chunckRenderer, chunckUpdateRect);

        chunckGroupStart += chunckGroupCount;
        t++;
    }

    printf("\nActive chuncks : %d  groups : %d\n", waterman->activeChunckCount, t);

    waterman->activeChunckCount = waterman->nextActiveChunckCount;
    qsort(waterman->nextActiveChuncks, waterman->activeChunckCount, sizeof(ChunckCoords), activeChunckComparator);

    memcpy(waterman->activeChuncks, waterman->nextActiveChuncks, waterman->activeChunckCount * sizeof(ChunckCoords));
}

void processWaterBloc(WaterManager* waterman, Terrain* terrain, Uint32 x, Uint32 y)
{
    Uint8 startValue = getLiquidLevel(terrain, x, y);
    Uint8 currentValue = startValue;
    Uint8 belowValue = getLiquidLevel(terrain, x, y+1);
    Uint8 upValue = getLiquidLevel(terrain, x, y-1);

    /*if(upValue > 0 && currentValue < WATER_LEVEL_COUNT)
    {
        Uint8 transfer = MIN(upValue, WATER_LEVEL_COUNT - currentValue);
        setLiquidLevel(terrain, x, y, currentValue + transfer);
        setLiquidLevel(terrain, x, y-1, upValue - transfer);
        currentValue += transfer;

        if(y % CHUNCK_HEIGHT == 0) {
            waterman_addActiveBloc(waterman, x, y-1);
            printf("addition up\n");
        }
    }*/

    if(checkTranferable(getBlock(terrain, x, y+1).type, belowValue))
    {
        Uint8 transfer = MIN(startValue, WATER_LEVEL_COUNT - belowValue);
        setLiquidLevel(terrain, x, y, startValue - transfer);
        setLiquidLevel(terrain, x, y+1, belowValue + transfer);
        currentValue -= transfer;

        if((y+1) % CHUNCK_HEIGHT == 0) {
            waterman_addActiveBloc(waterman, x, y+1);
            printf("addition down\n");
        }
    }
    else  //We can't go down
    {
        Uint8 waterLevel = startValue;

        Uint8 leftValue = getLiquidLevel(terrain, x-1, y);
        Uint8 rightValue = getLiquidLevel(terrain, x+1, y);

        if(checkTranferable(getBlock(terrain, x-1, y).type, leftValue))
        {
            if(checkTranferable(getBlock(terrain, x+1, y).type, rightValue))
            {
                int sideSum = startValue + leftValue + rightValue;
                waterLevel = sideSum / 3;
                int remainder = sideSum - 3*waterLevel;

                setLiquidLevel(terrain, x  , y, waterLevel);
                setLiquidLevel(terrain, x-1, y, waterLevel + (remainder > 0 ? 1 : 0));
                setLiquidLevel(terrain, x+1, y, waterLevel + (remainder > 1 ? 1 : 0));
            }
            else
            {
                Uint8 sideSum = startValue + leftValue;
                waterLevel = sideSum / 2;

                setLiquidLevel(terrain, x, y, waterLevel);
                setLiquidLevel(terrain, x-1, y, sideSum - waterLevel);
            }
        }
        else
        {
            if(checkTranferable(getBlock(terrain, x+1, y).type, rightValue))
            {
                Uint8 sideSum = startValue + rightValue;
                waterLevel = sideSum / 2;

                setLiquidLevel(terrain, x, y, waterLevel);
                setLiquidLevel(terrain, x+1, y, sideSum - waterLevel);
            }
        }
        currentValue = waterLevel;
    }

    if(upValue > 0 && currentValue < WATER_LEVEL_COUNT)
    {
        Uint8 transfer = MIN(upValue, WATER_LEVEL_COUNT - currentValue);
        setLiquidLevel(terrain, x, y, currentValue + transfer);
        setLiquidLevel(terrain, x, y-1, upValue - transfer);

        if(y % CHUNCK_HEIGHT == 0) {
            waterman_addActiveBloc(waterman, x, y-1);
            printf("addition up\n");
        }
    }

    if(startValue != getLiquidLevel(terrain, x, y))
    {
        if(x % CHUNCK_WIDTH == 0 && checkLiquid(terrain->blocTypes, getBlock(terrain, x-1, y)))
        {
            waterman_addActiveBloc(waterman, x-1, y);
            printf("addition left\n");
        }

        if((x+1) % CHUNCK_WIDTH == 0 && checkLiquid(terrain->blocTypes, getBlock(terrain, x+1, y)))
        {
            waterman_addActiveBloc(waterman, x+1, y);
            printf("addition right\n");
        }

        if(y % CHUNCK_HEIGHT == 0 && checkLiquid(terrain->blocTypes, getBlock(terrain, x, y-1))) {
            waterman_addActiveBloc(waterman, x, y-1);
            printf("addition up\n");
        }
    }
}

void waterman_addActiveBloc(WaterManager* waterman, Uint32 x, Uint32 y)
{
    addActiveChunck(waterman, getBlocChunckCoords(x, y));
}

void addActiveChunck(WaterManager* waterman, ChunckCoords coords)
{
    if(bsearch(&coords, waterman->nextActiveChuncks, waterman->nextActiveChunckCount, sizeof(ChunckCoords), activeChunckComparator) != NULL) return;

    waterman->nextActiveChuncks[waterman->nextActiveChunckCount++] = coords;
}

void deleteActiveChunck(WaterManager* waterman, Uint32 activeChunckID)
{
    waterman->nextActiveChuncks[activeChunckID] = waterman->nextActiveChuncks[--waterman->nextActiveChunckCount];
}

void setLiquidLevel(Terrain* terrain, Uint32 x, Uint32 y, Uint8 level)
{
    if(level == 0)
    {
        getBlockPtr(terrain, x, y)->type = BLOC_NONE;
        setBlocCustomValue(terrain, x, y, 0);
    }
    else
    {
        getBlockPtr(terrain, x, y)->type = BLOC_WATER;
        setBlocCustomValue(terrain, x, y, WATER_LEVEL_COUNT - level);
    }
}

Uint8 getLiquidLevel(Terrain* terrain, Uint32 x, Uint32 y)
{
    Uint8 type = getBlock(terrain, x, y).type;
    return (type == BLOC_WATER) ? WATER_LEVEL_COUNT - getBlocCustomValue(terrain, x, y) : 0;
}

int getExtendedLiquidLevel(Terrain* terrain, Uint32 x, Uint32 y)
{
    Uint8 type = getBlock(terrain, x, y-1).type;
    return (type == BLOC_WATER) ? WATER_LEVEL_COUNT+1 : WATER_LEVEL_COUNT - getBlocCustomValue(terrain, x, y);
}

bool checkTranferable(Uint8 type, Uint8 liquidLevel)
{
    return type == BLOC_NONE || (type == BLOC_WATER && liquidLevel < WATER_LEVEL_COUNT);
}

int activeChunckComparator(const void* a, const void* b)        //Descending order on Coordinates
{
    const ChunckCoords *arg1 = (const ChunckCoords*)a;
    const ChunckCoords *arg2 = (const ChunckCoords*)b;

    return (arg1->y == arg2->y) ? arg1->x - arg2->x : arg2->y - arg1->y;
}

int processContinuousActiveChuncks(ChunckCoords* activeChuncks, int activeChunckCount, Uint32 activeChunckID)
{
    Uint32 finalChunckID = activeChunckID;
    do finalChunckID++; while((activeChuncks[finalChunckID-1].y == activeChuncks[finalChunckID].y) &&
                              (activeChuncks[finalChunckID-1].x == activeChuncks[finalChunckID].x - 1));

    return finalChunckID - activeChunckID;
}

void waterman_destroy(WaterManager* waterman)
{
    free(waterman);
}
