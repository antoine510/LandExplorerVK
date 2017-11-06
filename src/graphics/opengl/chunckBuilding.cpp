#include "chunckBuilding.h"
#include "utility/mathUtility.h"
/*
static void buildBloc(Texture* blocTex, Uint8 lighting, Uint8 edges, Uint8 transparency, Uint32 x, Uint32 y);
static void buildChunck(ChunckBuilder* builder, Terrain* terrain, int textureID, SDL_Rect* subRect, SDL_Point blocOffset);

ChunckBuilder* chb_create(Texture* blocTex, Texture* backwallTex, TexturePack* texPack)
{
    ChunckBuilder* builder = calloc(1, sizeof(ChunckBuilder));

    builder->blocTex = blocTex;
    builder->backwallTex = backwallTex;
    builder->texPack = texPack;

    fb_init(&builder->blocBuffer, blocTex);
    fb_init(&builder->backwallBuffer, backwallTex);

    return builder;
}

void buildBloc(Texture* blocTex, Uint8 lighting, Uint8 edges, Uint8 transparency, Uint32 x, Uint32 y)
{
    SDL_Color colorMod = {lighting, lighting, lighting, 0};
    setTextureColorMod(blocTex, &colorMod);
    setTextureAlphaMod(blocTex, 255 - (transparency * (lighting+1) >> 8));
    setTextureClip(blocTex, edges%4, edges/4);
    setTexturePos(blocTex, x, y);

    drawTexture(blocTex);
}

void buildChunck(ChunckBuilder* builder, Terrain* terrain, int textureID, SDL_Rect* subRect, SDL_Point blocOffset)
{
    int dstX = (textureID / 8) * CHUNCK_WIDTH_PX;
    int dstY = (textureID % 8) * CHUNCK_HEIGHT_PX;

    int scissorX = dstX + subRect->x*BLOC_SIZE, scissorY = 8*CHUNCK_HEIGHT_PX - dstY - (subRect->y + subRect->h)*BLOC_SIZE;
    glScissor(dstX + subRect->x*BLOC_SIZE, 8*CHUNCK_HEIGHT_PX - dstY - (subRect->y + subRect->h)*BLOC_SIZE, subRect->w*BLOC_SIZE, subRect->h*BLOC_SIZE);
    int x, y;
    for(x = subRect->x - 1; x <= subRect->x + subRect->w; x++)
    {
        for(y = subRect->y - 1; y <= subRect->y + subRect->h; y++)
        {
            int blitX = dstX + x*BLOC_SIZE, blitY = dstY + y*BLOC_SIZE;

            Bloc* bloc = getBlockPtr(terrain, x + blocOffset.x, y + blocOffset.y);
            Bloc* backwall = getBackwallPtr(terrain, x + blocOffset.x, y + blocOffset.y);
            if(bloc->light != 0)
            {
                Uint8 lighting = ((bloc->light+1) << 4) -1;
                Uint8 blocEdges = getEdges(bloc, getType(terrain->blocTypes, *bloc));
                if(!builder->backwall && bloc->type != 0)
                {
                    Uint8 transparency = getTransparency(terrain->blocTypes, *bloc);
                    buildBloc(builder->texPack->terrain[bloc->type], lighting, blocEdges, transparency, blitX, blitY);
                }
                if(builder->backwall && (bloc->type == 0 || blocEdges != 0 || checkTransparent(terrain->blocTypes, *bloc)) && backwall->type != 0)
                {
                    Uint8 backwallEdges = getEdges(backwall, getType(terrain->blocTypes, *backwall));
                    buildBloc(builder->texPack->backwall[backwall->type], lighting, backwallEdges, 0, blitX - HALF_BLOC_SIZE, blitY - HALF_BLOC_SIZE);
                }
            }
            else if(!builder->backwall && (bloc->type != 0 || backwall->type != 0))
            {
                buildBloc(builder->texPack->black, 0, 0, 0, blitX, blitY);
            }
        }
    }
}

void chb_buildRect(ChunckBuilder* builder, Terrain* terrain, char* textureIDs, SDL_Rect* rect)
{
    int xChunck = rect->x / CHUNCK_WIDTH, yChunck = rect->y / CHUNCK_HEIGHT,
        x2Chunck = (rect->x+rect->w-1) / CHUNCK_WIDTH +1, y2Chunck = (rect->y+rect->h-1) / CHUNCK_HEIGHT +1;

    int x, y;
    for(x = xChunck; x < x2Chunck; x++)
    {
        for(y = yChunck; y < y2Chunck; y++)
        {
            int textureID = textureIDs[x * terrain->heightChunck + y];
            int xSeg = rect->x - x*CHUNCK_WIDTH, ySeg = rect->y - y*CHUNCK_HEIGHT;
            SDL_Rect subRect = {max(0, xSeg), max(0, ySeg),
                                min(xSeg + rect->w, CHUNCK_WIDTH) - max(0,xSeg),
                                min(ySeg + rect->h, CHUNCK_HEIGHT) - max(0,ySeg)};

            if(textureIDs[x * terrain->heightChunck + y] != -1)
                buildChunck(builder, terrain, textureID, &subRect, (SDL_Point){x*CHUNCK_WIDTH, y*CHUNCK_HEIGHT});
        }
    }
}

void chb_startBuilding(ChunckBuilder* builder, bool backwall)
{
    glEnable(GL_SCISSOR_TEST);
    fb_startRendering(backwall ? &builder->backwallBuffer : &builder->blocBuffer);
    builder->backwall = backwall;
}

void chb_endBuilding()
{
    fb_endRendering();
    glDisable(GL_SCISSOR_TEST);
}

void chb_destroy(ChunckBuilder* builder)
{
    fb_destroy(&builder->blocBuffer);
    fb_destroy(&builder->backwallBuffer);

    free(builder);
}
*/
