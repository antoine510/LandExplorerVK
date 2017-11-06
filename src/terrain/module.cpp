#include "module.h"
#include <cstring>
#include "../mathUtility.h"
#include "terrainGeneration.h"
#include "../level.h"
#include "../entityModels.h"
#include "../lightSource.h"

static Module* createModule(SDL_Point size);

Module* createModule(SDL_Point size)
{
    Module* module = calloc(1, sizeof(Module));

    module->size = size;
    module->blocks = malloc(size.x * size.y * sizeof(Uint8));
    module->backwall = malloc(size.x * size.y * sizeof(Uint8));

    return module;
}

void destroyModule(Module* module)
{
    free(module->blocks);
    free(module->backwall);
    free(module);
}

Module* getModuleFromTerrain(Terrain* terrain, SDL_Rect rect)
{
    Module* module = createModule((SDL_Point){rect.w, rect.h});

    int x, y;
    for (x = 0; x < rect.w; x++)
    {
        for (y = 0; y < rect.h; y++)
        {
            module->blocks[x * rect.h + y] = getBlock(terrain, x + rect.x, y + rect.y).type;
            module->backwall[x * rect.h + y] = getBackwall(terrain, x + rect.x, y + rect.y).type;
        }
    }

    return module;
}

void saveModule(Module* module, const char* moduleName)
{
    char fileName[MAX_MODULE_FILENAME_LENGTH] = "modules/";
    strcat(fileName, moduleName);
    strcat(fileName, ".mod");

    FILE *f;
    f = fopen(fileName, "wb");

    SDL_Point param[2] = {module->size, module->ref};
    fwrite(param, sizeof(SDL_Point), 2, f);

    int counts[2] = {module->archerCount, module->guardCount};
    fwrite(counts, sizeof(int), 2, f);

    fwrite(module->archerPos, sizeof(SDL_Point), module->archerCount, f);
    fwrite(module->guardPos, sizeof(SDL_Point), module->guardCount, f);

    fwrite(module->blocks, sizeof(Uint8), module->size.x * module->size.y, f);
    fwrite(module->backwall, sizeof(Uint8), module->size.x * module->size.y, f);
    fclose(f);
}

void pasteModule(Module* module, Level* level, SDL_Point point, bool processConditions, bool mirror)
{
    int x, y;
    SDL_Rect t = {point.x - module->ref.x, point.y - module->ref.y, module->size.x, module->size.y};
    clampWorld(level->terrain->width, level->terrain->height, &t, TERRAIN_BORDER);
    for (x = 0; x < t.w; x++)
    {
        int tx = mirror ? (t.w - x -1) : x;
        for (y = 0; y < t.h; y++)
        {
            getBlockPtr(level->terrain, x + t.x, y + t.y)->type = module->blocks[tx * module->size.y + y];
            getBackwallPtr(level->terrain, x + t.x, y + t.y)->type = module->backwall[tx * module->size.y + y];
            if(module->blocks[tx * module->size.y + y] == BLOC_TORCH) addTorch(level->terrain, x + t.x, y + t.y);
        }
    }
    if(processConditions)
    {
        processSunLightAroundRect(level->terrain, t);  //Does include chunckenderer updating
    }

    int i;
    for(i = 0; i < module->archerCount; i++)
    {
        Vect2 archerPos = (Vect2){(float)module->archerPos[i].x, (float)module->archerPos[i].y};
        archerPos.x += point.x - module->ref.x; archerPos.y += point.y - module->ref.y;

        int archerID = getFreeEntity(level->entities);
        setEntityAsModel(level->entities, archerID, "archer");

        archerPos.y -= level->entities->collData[archerID].sz.y;

        setCollisionPos(level->entities, archerID, (Vect2){archerPos.x, archerPos.y});
        level->entities->aiData[archerID].playerID = level->playerID;
    }
    for(i = 0; i < module->guardCount; i++)
    {
        SDL_Point guardPos = module->guardPos[i];
        guardPos.x += point.x - module->ref.x; guardPos.y += point.y - module->ref.y;

        int guardID = getFreeEntity(level->entities);
        setEntityAsModel(level->entities, guardID, "guard");

        guardPos.y -= (int)level->entities->collData[guardID].sz.y;

        setCollisionPos(level->entities, guardID, (Vect2){(float)guardPos.x, (float)guardPos.y});
        level->entities->aiData[guardID].playerID = level->playerID;
    }
}

Module* loadModule(const char* moduleName)
{
    char fileName[MAX_MODULE_FILENAME_LENGTH] = "modules/";
    strcat(fileName, moduleName);
    strcat(fileName, ".mod");

    FILE *f;
    f = fopen(fileName, "rb");
    #ifdef DEBUG
    if(f == NULL) fprintf(stderr, "Error : The module %s couldn't be found\n", moduleName);
    #endif // DEBUG

    SDL_Point param[2];
    fread(param, sizeof(SDL_Point), 2, f);
    int counts[2];
    fread(counts, sizeof(int), 2, f);

    Module* module = createModule(param[0]);
    module->ref = param[1];
    module->archerCount = counts[0]; module->guardCount = counts[1];

    fread(module->archerPos, sizeof(SDL_Point), module->archerCount, f);
    fread(module->guardPos, sizeof(SDL_Point), module->guardCount, f);

    fread(module->blocks, sizeof(Uint8), module->size.x * module->size.y, f);
    fread(module->backwall, sizeof(Uint8), module->size.x * module->size.y, f);
    fclose(f);

    return module;
}

