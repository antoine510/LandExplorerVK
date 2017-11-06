#include "chunkRenderer.h"
#include "terrain/blockTypes.h"
#include "terrain/terrainGeneration.h"
#include "utility/mathUtility.h"
#include "displayInfo.h"
#include <algorithm>

/*typedef struct {
    ChunckRenderer* chunckRenderer;
    SDL_Surface* blocSurface;
    SDL_Surface* backwallSurface;
    SDL_Rect rect;
    Uint8 surfaceXOffset;
} MT_updateRectParams;*/

//int chr_MT_updateRect(void* data);

static int createChunck(ChunckRenderer* cRenderer, int x, int y);
static void updateBloc(SDL_Surface* surface, SDL_Surface* blocTex, Uint8 lighting, Uint8 edges, Uint8 transparency, Uint32 x, Uint32 y);
static void updateBackwall(SDL_Surface* dstSurface, SDL_Surface* blocTex, Uint8 lighting, Uint8 edges, Uint32 x, Uint32 y);
static void blitBlack(SDL_Surface* surface, SDL_Surface* black, Uint32 x, Uint32 y);
static void processChunckViewport(ChunckRenderer* cRenderer);

ChunckRenderer* initChunckRenderer(TexturePack* texPack, SDL_Point* viewOrigin)
{
    ChunckRenderer* cRenderer = (ChunckRenderer*)malloc(sizeof(ChunckRenderer));
    cRenderer->textureID = NULL;
    cRenderer->texPack = texPack;
    cRenderer->viewOrigin = viewOrigin;

    cRenderer->blocTex = createEmptyTexture(cRenderer->texPack->renderer, 8*CHUNCK_WIDTH_PX, 8*CHUNCK_HEIGHT_PX);
    cRenderer->backwallTex = createEmptyTexture(cRenderer->texPack->renderer, 8*CHUNCK_WIDTH_PX, 8*CHUNCK_HEIGHT_PX);

    SDL_Surface* black = SDL_CreateRGBSurface(0, CHUNCK_WIDTH_PX, CHUNCK_HEIGHT_PX,
                                              32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_FillRect(black, NULL, 0xff000000);
    SDL_Rect updateRect = {7*CHUNCK_WIDTH_PX, 7*CHUNCK_HEIGHT_PX, CHUNCK_WIDTH_PX, CHUNCK_HEIGHT_PX};
    updateTexture(cRenderer->blocTex, &updateRect, black->pixels, black->pitch);
    updateTexture(cRenderer->backwallTex, &updateRect, black->pixels, black->pitch);
    SDL_FreeSurface(black);

    cRenderer->xC1 = 0; cRenderer->xC2 = 0; cRenderer->yC1 = 0; cRenderer->yC2 = 0;

    int i;
    for(i = 0; i < MAX_TEXTURE_STORAGE; i++) cRenderer->texturePosition[i] = -1;
    cRenderer->currentTextureID = 0;

    cRenderer->chunckGroup = createTextureGroup(cRenderer->texPack->renderer, CHUNCK_WIDTH_PX, CHUNCK_HEIGHT_PX, 8, 8,
                                              myDisplayMode.w/CHUNCK_WIDTH_PX +2,
                                              myDisplayMode.h/CHUNCK_HEIGHT_PX +2,
                                              2, "chunckRender", shader_chunck);
    setTextureGroupTextureBank(cRenderer->chunckGroup, cRenderer->blocTex, 0);
    setTextureGroupTextureBank(cRenderer->chunckGroup, cRenderer->backwallTex, 1);
    setTextureGroupColorMod(cRenderer->chunckGroup, &cRenderer->texPack->skyColor);

    //cRenderer->builder = chb_create(cRenderer->blocTex, cRenderer->backwallTex, texPack);

    return cRenderer;
}

void setTerrain(ChunckRenderer* chunckRenderer, Terrain* terrain)
{
    chunckRenderer->terrain = terrain;
    chunckRenderer->textureID = (char*)malloc(terrain->widthChunck * terrain->heightChunck * sizeof(char));
    memset(chunckRenderer->textureID, -1, terrain->widthChunck * terrain->heightChunck * sizeof(char));
}

int createChunck(ChunckRenderer* cRenderer, int x, int y)
{
    int chunckTextureID = cRenderer->currentTextureID;

    //If the position is already in use
    if(cRenderer->texturePosition[chunckTextureID] != -1)
        cRenderer->textureID[cRenderer->texturePosition[chunckTextureID]] = -1;

    cRenderer->textureID[x * cRenderer->terrain->heightChunck + y] = chunckTextureID;
    cRenderer->texturePosition[chunckTextureID] = x * cRenderer->terrain->heightChunck + y;
    cRenderer->currentTextureID++;
	if(cRenderer->currentTextureID == MAX_TEXTURE_STORAGE) cRenderer->currentTextureID = 0;

    SDL_Rect updateChunck = {x*CHUNCK_WIDTH, y*CHUNCK_HEIGHT, CHUNCK_WIDTH, CHUNCK_HEIGHT};
    updateRect(cRenderer, updateChunck);

    return chunckTextureID;
}

/*void chr_updateRect(ChunckRenderer* chunckRenderer, SDL_Rect rect)
{
    rect.x--; rect.y--; rect.w += 2; rect.h += 2;
    SDL_Surface* updatedBlocSrf = SDL_CreateRGBSurface(0, rect.w * BLOC_SIZE, rect.h * BLOC_SIZE, 32,
                                                        0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_Surface* updatedBackwallSrf = SDL_CreateRGBSurface(0, rect.w * BLOC_SIZE, rect.h * BLOC_SIZE, 32,
                                                            0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

    int sectorCount = 4;//SDL_GetCPUCount();
    SDL_Thread* threads[sectorCount];
    float xSector = (0.1f + rect.w) / sectorCount, currentX = rect.x;

    MT_updateRectParams baseParams = {chunckRenderer, updatedBlocSrf, updatedBackwallSrf, {rect.x, rect.y, (int)xSector, rect.h}, 0};
    MT_updateRectParams params[sectorCount];

    #ifdef PERFLOG
    int t1 = SDL_GetTicks();
    #endif // PERFLOG
    int i;
    for(i = 0; i < sectorCount; i++)
    {
        baseParams.rect.x = (int)currentX;
        baseParams.rect.w = (int)(currentX + xSector) - baseParams.rect.x;

        params[i] = baseParams;
        threads[i] = SDL_CreateThread(chr_MT_updateRect, "ChunckRenderer", &params[i]);
        if(threads[i] == NULL) fprintf(stderr, "Error : Failed to create chunckRenderer thread");
        //chr_MT_updateRect(&params);

        baseParams.surfaceXOffset += baseParams.rect.w;
        currentX += xSector;
    }
    for(i = 0; i < sectorCount; i++)
    {
        SDL_WaitThread(threads[i], NULL);
    }
    #ifdef PERFLOG
    int t2 = SDL_GetTicks();
    #endif // PERFLOG

    rect.x++; rect.y++; rect.w -= 2; rect.h -= 2;
    int xChunck = rect.x / CHUNCK_WIDTH, yChunck = rect.y / CHUNCK_HEIGHT,
        x2Chunck = (rect.x+rect.w-1) / CHUNCK_WIDTH +1, y2Chunck = (rect.y+rect.h-1) / CHUNCK_HEIGHT +1;
    int x, y;
    for(x = xChunck; x < x2Chunck; x++)
    {
        for(y = yChunck; y < y2Chunck; y++)
        {
            int xSeg = rect.x - x*CHUNCK_WIDTH, ySeg = rect.y - y*CHUNCK_HEIGHT;
            int xoff = max(0, xSeg) - xSeg +1, yoff = max(0, ySeg) - ySeg +1;
            SDL_Rect updateRect = {max(0, xSeg)*BLOC_SIZE, max(0, ySeg)*BLOC_SIZE,
                                    (min(xSeg + rect.w, CHUNCK_WIDTH) - max(0,xSeg))*BLOC_SIZE,
                                    (min(ySeg + rect.h, CHUNCK_HEIGHT) - max(0,ySeg))*BLOC_SIZE};

            Uint8* pixelAdress = (Uint8*)updatedBlocSrf->pixels + yoff*BLOC_SIZE*updatedBlocSrf->pitch + xoff*BLOC_SIZE*sizeof(Uint32);
            SDL_UpdateTexture(chunckRenderer->chuncksTex[x * chunckRenderer->terrain->heightChunck + y], &updateRect, pixelAdress, updatedBlocSrf->pitch);

            pixelAdress = (Uint8*)updatedBackwallSrf->pixels + yoff*BLOC_SIZE*updatedBackwallSrf->pitch + xoff*BLOC_SIZE*sizeof(Uint32);
            SDL_UpdateTexture(chunckRenderer->backwallTex[x * chunckRenderer->terrain->heightChunck + y], &updateRect, pixelAdress, updatedBackwallSrf->pitch);

            chunckRenderer->isEmptyBlack[x * chunckRenderer->terrain->heightChunck + y] = isTerrainChunckEmpty(chunckRenderer->terrain, x, y) +
                                                                    2*isTerrainChunckBlack(chunckRenderer->terrain, x, y);
        }
    }

    SDL_FreeSurface(updatedBlocSrf);
    SDL_FreeSurface(updatedBackwallSrf);

    #ifdef PERFLOG
    int t3 = SDL_GetTicks();
    static int count = 1;
    static float time1 = 0, time2 = 0;
    time1 += t2 - t1; time2 += t3 - t2;
    printf("Created surface : now :%d,%d average : %f,%f taken on %d samples\n\n", t2-t1, t3-t2, time1/count, time2/count, count);
    count++;
    #endif // PERFLOG
}*/

/*int chr_MT_updateRect(void* data)
{
    int srfXOffset = ((MT_updateRectParams*)data)->surfaceXOffset;
    ChunckRenderer* chunckRenderer = ((MT_updateRectParams*)data)->chunckRenderer;
    SDL_Surface* updatedBlocSrf = ((MT_updateRectParams*)data)->blocSurface;
    SDL_Surface* updatedBackwallSrf = ((MT_updateRectParams*)data)->backwallSurface;
    SDL_Rect rect = ((MT_updateRectParams*)data)->rect;
    int x, y;
    for(x = rect.x; x < rect.x+rect.w; x++)
    {
        for(y = rect.y; y < rect.y+rect.h; y++)
        {
            Bloc* bloc = getBlockPtr(chunckRenderer->terrain, x, y), *backwall = getBackwallPtr(chunckRenderer->terrain, x, y);
            SDL_Color ligthing = processLighting(chunckRenderer->terrain, chunckRenderer->texPack->skyColor, bloc->light, x, y);
            if(ligthing.a != 0)
            {
                Uint8 blocEdges = getEdges(bloc, getType(chunckRenderer->terrain->blocTypes, *bloc));
                if(bloc->type != 0)
                    updateBloc(updatedBlocSrf, chunckRenderer->texPack->terrain[bloc->type], ligthing, blocEdges, x - rect.x + srfXOffset, y - rect.y);
                if((bloc->type == 0 || blocEdges != 0 || checkTransparent(chunckRenderer->terrain->blocTypes, *bloc)) && backwall->type != 0)
                {
                    Uint8 backwallEdges = getEdges(backwall, getType(chunckRenderer->terrain->blocTypes, *backwall));
                    updateBackwall(updatedBackwallSrf, chunckRenderer->texPack->backwall[backwall->type], ligthing, backwallEdges, x - rect.x + srfXOffset, y - rect.y);
                }
            }
            else if(bloc->type != 0 || backwall->type != 0)
            {
                blitBlack(updatedBlocSrf, chunckRenderer->texPack->black, x - rect.x + srfXOffset, y - rect.y);
            }
        }
    }
    return 0;
}*/

void updateRect(ChunckRenderer* chunckRenderer, SDL_Rect rect)
{
    rect.x--; rect.y--; rect.w += 2; rect.h += 2;
    SDL_Surface* updatedBlocSrf = SDL_CreateRGBSurface(0, rect.w * BLOC_SIZE, rect.h * BLOC_SIZE, 32,
                                                        0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    SDL_Surface* updatedBackwallSrf = SDL_CreateRGBSurface(0, rect.w * BLOC_SIZE, rect.h * BLOC_SIZE, 32,
                                                            0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    int x, y;
    #ifdef PERFLOG
    int t1 = SDL_GetTicks();
    #endif // PERFLOG
    for(x = rect.x; x < rect.x+rect.w; x++)
    {
        for(y = rect.y; y < rect.y+rect.h; y++)
        {
            Bloc* bloc = getBlockPtr(chunckRenderer->terrain, x, y), *backwall = getBackwallPtr(chunckRenderer->terrain, x, y);
            if(bloc->light != 0)
            {
                Uint8 lighting = ((bloc->light+1) << 4) -1;
                Uint8 blocEdges = chunckRenderer->terrain->blocTypes[bloc->type].getBlocTypeCustomValue(chunckRenderer->terrain, bloc);
                if(bloc->type != 0)
                {
                    Uint8 transparency = getTransparency(chunckRenderer->terrain->blocTypes, *bloc);
                    updateBloc(updatedBlocSrf, chunckRenderer->texPack->terrain[bloc->type], lighting, blocEdges, transparency, x - rect.x, y - rect.y);
                }
                if((bloc->type == 0 || blocEdges != 0 || checkTransparent(chunckRenderer->terrain->blocTypes, *bloc)) && backwall->type != 0)
                {
                    Uint8 backwallEdges = chunckRenderer->terrain->blocTypes[backwall->type].getBlocTypeCustomValue(chunckRenderer->terrain, backwall);
                    updateBackwall(updatedBackwallSrf, chunckRenderer->texPack->backwall[backwall->type], lighting, backwallEdges, x - rect.x, y - rect.y);
                }
            }
            else if(bloc->type != 0 || backwall->type != 0)
            {
                blitBlack(updatedBlocSrf, chunckRenderer->texPack->black, x - rect.x, y - rect.y);
            }
        }
    }
    #ifdef PERFLOG
    int t2 = SDL_GetTicks();
    #endif // PERFLOG
    rect.x++; rect.y++; rect.w -= 2; rect.h -= 2;
    int xChunck = rect.x / CHUNCK_WIDTH, yChunck = rect.y / CHUNCK_HEIGHT,
        x2Chunck = (rect.x+rect.w-1) / CHUNCK_WIDTH +1, y2Chunck = (rect.y+rect.h-1) / CHUNCK_HEIGHT +1;
    for(x = xChunck; x < x2Chunck; x++)
    {
        for(y = yChunck; y < y2Chunck; y++)
        {
            int textureID = chunckRenderer->textureID[x * chunckRenderer->terrain->heightChunck + y];
            int xSeg = rect.x - x*CHUNCK_WIDTH, ySeg = rect.y - y*CHUNCK_HEIGHT;
            int xoff = std::max(0, xSeg) - xSeg +1, yoff = std::max(0, ySeg) - ySeg +1;
            SDL_Rect updateRect = { std::max(0, xSeg)*BLOC_SIZE, std::max(0, ySeg)*BLOC_SIZE,
                                    (std::min(xSeg + rect.w, CHUNCK_WIDTH) - std::max(0,xSeg))*BLOC_SIZE,
                                    (std::min(ySeg + rect.h, CHUNCK_HEIGHT) - std::max(0,ySeg))*BLOC_SIZE};
            updateRect.x += (textureID / 8) * CHUNCK_WIDTH_PX;
            updateRect.y += (textureID % 8) * CHUNCK_HEIGHT_PX;

            if(chunckRenderer->textureID[x * chunckRenderer->terrain->heightChunck + y] != -1) {
                Uint8* pixelAdress = (Uint8*)updatedBlocSrf->pixels + yoff*BLOC_SIZE*updatedBlocSrf->pitch + xoff*BLOC_SIZE*sizeof(Uint32);
                updateTexture(chunckRenderer->blocTex, &updateRect, pixelAdress, updatedBlocSrf->pitch);

                pixelAdress = (Uint8*)updatedBackwallSrf->pixels + yoff*BLOC_SIZE*updatedBackwallSrf->pitch + xoff*BLOC_SIZE*sizeof(Uint32);
                updateTexture(chunckRenderer->backwallTex, &updateRect, pixelAdress, updatedBackwallSrf->pitch);
            }
        }
    }

    #ifdef PERFLOG
    int t3 = SDL_GetTicks();
    #endif // PERFLOG
    SDL_FreeSurface(updatedBlocSrf);
    SDL_FreeSurface(updatedBackwallSrf);

    processChunckViewport(chunckRenderer);
    #ifdef PERFLOG
    static int count = 1;
    static float time1 = 0, time2 = 0;
    time1 += t2 - t1; time2 += t3 - t2;
    printf("Update surface : current :%d,%d average : %f,%f taken on %d samples\n", t2-t1, t3-t2, time1/count, time2/count, count);
    count++;
    #endif // PERFLOG
}

/*void updateRect(ChunckRenderer* cRenderer, SDL_Rect rect)
{
    chb_startBuilding(chunckRenderer->builder, false);
    chb_buildRect(chunckRenderer->builder, cRenderer->terrain, cRenderer->textureID, &rect);
    chb_startBuilding(chunckRenderer->builder, true);
    chb_buildRect(chunckRenderer->builder, cRenderer->terrain, cRenderer->textureID, &rect);
    chb_endBuilding();
}*/

void updatePos(ChunckRenderer* chunckRenderer, Uint32 x, Uint32 y)
{
    SDL_Rect r = {x, y, 1, 1};
    updateRect(chunckRenderer, r);
}

void updateBloc(SDL_Surface* dstSurface, SDL_Surface* blocTex, Uint8 lighting, Uint8 edges, Uint8 transparency, Uint32 x, Uint32 y)
{
    SDL_Rect dstRect = {x * BLOC_SIZE, y * BLOC_SIZE, BLOC_SIZE, BLOC_SIZE};
    SDL_SetSurfaceColorMod(blocTex, lighting, lighting, lighting);
    if(transparency != 0) SDL_SetSurfaceAlphaMod(blocTex, 255 - (transparency * (lighting+1) >> 8));
    SDL_Rect clip = {(edges%4)*BLOC_SIZE, edges/4*BLOC_SIZE, BLOC_SIZE, BLOC_SIZE};

    SDL_BlitSurface(blocTex, &clip, dstSurface, &dstRect);
}

void updateBackwall(SDL_Surface* dstSurface, SDL_Surface* blocTex, Uint8 lighting, Uint8 edges, Uint32 x, Uint32 y)
{
    SDL_Rect dstRect = {x * BLOC_SIZE - HALF_BLOC_SIZE, y * BLOC_SIZE - HALF_BLOC_SIZE, 2*BLOC_SIZE, 2*BLOC_SIZE};
    SDL_SetSurfaceColorMod(blocTex, lighting, lighting, lighting);
    SDL_Rect clip = {(edges%4)*2*BLOC_SIZE, edges/4*2*BLOC_SIZE, 2*BLOC_SIZE, 2*BLOC_SIZE};

    SDL_BlitSurface(blocTex, &clip, dstSurface, &dstRect);
}

void blitBlack(SDL_Surface* surface, SDL_Surface* black, Uint32 x, Uint32 y)
{
    SDL_Rect dstRect = {x * BLOC_SIZE, y * BLOC_SIZE, BLOC_SIZE, BLOC_SIZE};
    SDL_BlitSurface(black, NULL, surface, &dstRect);
}

void renderBlocScreen(ChunckRenderer* chunckRenderer)
{
    drawTextureGroup(chunckRenderer->chunckGroup, chunckRenderer->xC1*CHUNCK_WIDTH_PX - chunckRenderer->viewOrigin->x,
													chunckRenderer->yC1*CHUNCK_HEIGHT_PX - chunckRenderer->viewOrigin->y, 0);
}

void renderBackwallScreen(ChunckRenderer* chunckRenderer)
{
    drawTextureGroup(chunckRenderer->chunckGroup, chunckRenderer->xC1*CHUNCK_WIDTH_PX - chunckRenderer->viewOrigin->x,
													chunckRenderer->yC1*CHUNCK_HEIGHT_PX - chunckRenderer->viewOrigin->y, 1);
}

void updateChunckRenderer(ChunckRenderer* chunckRenderer)
{
    int xChunck = (chunckRenderer->viewOrigin->x - myDisplayMode.w/2) / CHUNCK_WIDTH_PX,
        yChunck = (chunckRenderer->viewOrigin->y - myDisplayMode.h/2) / CHUNCK_HEIGHT_PX,
        x2Chunck = (chunckRenderer->viewOrigin->x + myDisplayMode.w/2) /CHUNCK_WIDTH_PX,
        y2Chunck = (chunckRenderer->viewOrigin->y + myDisplayMode.h/2) /CHUNCK_HEIGHT_PX;

    if(chunckRenderer->xC1 != xChunck || chunckRenderer->yC1 != yChunck ||
       chunckRenderer->xC2 != x2Chunck || chunckRenderer->yC2 != y2Chunck)
    {
        chunckRenderer->xC1 = xChunck; chunckRenderer->yC1 = yChunck; chunckRenderer->xC2 = x2Chunck; chunckRenderer->yC2 = y2Chunck;
        processChunckViewport(chunckRenderer);
    }
}

void processChunckViewport(ChunckRenderer* cRenderer)
{
    clearTextureGroup(cRenderer->chunckGroup);
    clearChunckShaderLights((ChunckShaderUniforms*)cRenderer->chunckGroup->groupShader->uniforms);
    int x, y;
    for(x = cRenderer->xC1; x <= cRenderer->xC2; x++)
    {
        for(y = cRenderer->yC1; y <= cRenderer->yC2; y++)
        {
            if(getChunckPtr(cRenderer->terrain, x, y)->empty) {}
            else if(getChunckPtr(cRenderer->terrain, x, y)->black)
            {
                setTextureInGroup(cRenderer->chunckGroup, MAX_TEXTURE_STORAGE, (x - cRenderer->xC1), (y - cRenderer->yC1));
            }
            else
            {
                int textureID = cRenderer->textureID[x * chunckRenderer->terrain->heightChunck + y];

                if(textureID == -1) textureID = createChunck(chunckRenderer, x, y);

                setTextureInGroup(cRenderer->chunckGroup, textureID, (x - cRenderer->xC1), (y - cRenderer->yC1));
                int i;
                for(i = 0; i < cRenderer->terrain->chuncks[getChunck(x, y)].lightCount; i++)
                {
                    LightSource* ls = &cRenderer->terrain->chuncks[getChunck(x, y)].lights[i];
                    Uint32 posX, posY;
                    getLightSourcePosition(ls, &posX, &posY);
                    addChunckShaderLight((ChunckShaderUniforms*)cRenderer->chunckGroup->groupShader->uniforms,
                                         (float)posX / CHUNCK_WIDTH_PX - cRenderer->xC1,
                                         (float)posY / CHUNCK_HEIGHT_PX - cRenderer->yC1,
                                         ls->color);
                }
            }
        }
    }
}

void destroyChunckRenderer(ChunckRenderer* chunckRenderer)
{
    destroyTexture(chunckRenderer->blocTex);
    destroyTexture(chunckRenderer->backwallTex);
    destroyTextureGroup(chunckRenderer->chunckGroup);

    free(chunckRenderer->textureID);
    free(chunckRenderer);
}

