#include "texturePack.h"
#include "graphics.h"
#include "utility/xmlTools.h"
#include "terrain/blockTypes.h"
#include <SDL_image.h>

TexturePack* initTexturePack(Graphics* gfx)
{
    TexturePack* texPack = (TexturePack*)calloc(1, sizeof(TexturePack));

    int texSetCount = 0, buttonSetCount = 0;

    xmlDocPtr texturesDoc = parseXML("textures/textures.xml");
	xmlNodePtr texture = xmlDocGetRootElement(texturesDoc)->xmlChildrenNode;
	while(texture->type == XML_TEXT_NODE) texture = texture->next;

	while (texture)
    {
		std::string filename("textures/");
		filename += asStringl(texture, "path");
		if (checkName(texture, "terrain")) {
            int blocID = asIntl(texture, "id");
		    texPack->terrain[blocID] = IMG_Load(filename.c_str());
		} else if (checkName(texture, "backwall")) {
		    int blocID = asIntl(texture, "id");
		    texPack->backwall[blocID] = IMG_Load(filename.c_str());
		} else if (checkName(texture, "breakingBloc")) {
		    texPack->breakingBloc = createTextureFromFile(texPack->renderer, filename.c_str());
		    setTextureClipSize(texPack->breakingBloc, BLOC_SIZE, BLOC_SIZE);
		} else if (checkName(texture, "playerHearth")) {
		    texPack->playerHearth = createTextureFromFile(texPack->renderer, filename.c_str());
		} else if (checkName(texture, "itemSlot")) {
		    texPack->itemSlot = createTextureFromFile(texPack->renderer, filename.c_str());
		} else if (checkName(texture, "itemAtlas")) {
		    texPack->itemAtlas = createTextureFromFile(texPack->renderer, filename.c_str());
		    setTextureClipSize(texPack->itemAtlas, TEXTURE_ITEM_SIZE, TEXTURE_ITEM_SIZE);
		} else if (checkName(texture, "backgroundRenderer")) {
            backgroundRendererLoadTextures(gfx->bgRenderer, texPack->renderer, &texPack->skyColor, texture);
        } else if (checkName(texture, "menuRenderer")) {
            menuRendererLoadTextures(&gfx->menuRenderer, texPack->renderer, texture);
		} else if (checkName(texture, "texSet")) {
		    if(texSetCount >= MAX_TEXTURESET_COUNT) printf("Error while assigning textureSet, please increase MAX_TEXTURESET_COUNT");
            texPack->texSets[texSetCount] = createTextureSet(texPack->renderer, texture, filename.c_str());
            setTexSetColorMod(texPack->texSets[texSetCount], &texPack->skyColor);
            texSetCount++;
		} else if (checkName(texture, "buttonSet")) {
		    if(buttonSetCount >= MAX_BUTTONSET_COUNT) printf("Error while assigning buttonSet, please increase MAX_BUTTONSET_COUNT");
            texPack->buttonSets[buttonSetCount] = createTextureSet(texPack->renderer, texture, filename.c_str());
            buttonSetCount++;
		}

        do texture = texture->next; while(texture && texture->type == XML_TEXT_NODE);
	}
	xmlFreeDoc(texturesDoc);

    return texPack;
}

void blitBreak(TexturePack* texPack, int breakingStep, Uint32 x, Uint32 y)
{
    setTextureClip(texPack->breakingBloc, breakingStep, 0);
    setTexturePos(texPack->breakingBloc, x, y);
    drawTexture(texPack->breakingBloc);
}

void blitMapIcon(TexturePack* texPack, GfxData* gfxData, SDL_Rect rect, float scalingFactor, SDL_Point panningPos)
{
    setTexturePos(texPack->texSets[gfxData->texID]->mapIcon,
                            (int)((rect.x + rect.w/2) * scalingFactor / BLOC_SIZE) - panningPos.x,
                            (int)(TERRAIN_HEIGHT * scalingFactor - (rect.y + rect.h/2) * scalingFactor / BLOC_SIZE) - panningPos.y);
    drawTexture(texPack->texSets[gfxData->texID]->mapIcon);
}

void blitEntity(TexturePack* texPack, GfxData* gfxData, SDL_Rect rect)
{
    Texture* tex = getTexture(texPack->texSets[gfxData->texID], gfxData->texState);
    setTexturePos(tex, rect.x, rect.y);
    setTextureAngle(tex, gfxData->angle);
    drawTexture(tex);
}

/** \brief Draws a hearth on screen
 *
 * \param texPack The texture pack which contains the hearth texture
 * \param rect The drawing position
 * \param ratioFull The ratio to a full hearth
 */
void blitHearth(TexturePack* texPack, SDL_Point pos, float ratioFull)
{
    setTextureAlphaMod(texPack->playerHearth, (Uint8)(255 * ratioFull));
    setTexturePos(texPack->playerHearth, pos.x, pos.y);
    drawTexture(texPack->playerHearth);
}

/** \brief Draws an item slot on screen
 *
 * \param texPack The texture pack which contains the item slot texture
 * \param rect The drawing position
 */
void blitSlot(TexturePack* texPack, SDL_Point pos, float sizeMul)
{
    setTexturePos(texPack->itemSlot, pos.x, pos.y);
    setTextureScale(texPack->itemSlot, sizeMul);
    drawTexture(texPack->itemSlot);
}

/** \brief Draws an item on screen
 *
 * \param texPack The texture pack which contains the item texture
 * \param rect The drawing position
 * \param itemID The item ID
 */
void blitItem(TexturePack* texPack, SDL_Point pos, int itemID)
{
    setTextureClip(texPack->itemAtlas, itemID % 32, itemID / 32);
    setTexturePos(texPack->itemAtlas, pos.x, pos.y);
    drawTexture(texPack->itemAtlas);
}

/** \brief Resets all texture set current users
 *
 * \param texPack The texture pack which contains the texture sets
 */
void texPackFrameReset(TexturePack* texPack)
{
    int i;
    for(i = 0; i < MAX_TEXTURESET_COUNT; i++)
    {
        if(texPack->texSets[i] != NULL) texSetFrameReset(texPack->texSets[i]);
    }
}

/** \brief Destroys a texture pack
 *
 * \param texPack The texture pack to be destroyed
 */
void destroyTexturePack(TexturePack* texPack)
{
    int i;
    for(i = 0; i < BLOC_TYPE_COUNT; i++)
    {
        if(texPack->terrain[i] != NULL) SDL_FreeSurface(texPack->terrain[i]);
        if(texPack->backwall[i] != NULL) SDL_FreeSurface(texPack->backwall[i]);
    }

    for(i = 0; i < MAX_TEXTURESET_COUNT; i++)
    {
        if(texPack->texSets[i] != NULL) destroyTextureSet(texPack->texSets[i]);
    }
    for(i = 0; i < MAX_BUTTONSET_COUNT; i++)
    {
        if(texPack->buttonSets[i] != NULL) destroyTextureSet(texPack->buttonSets[i]);
    }

    destroyTexture(texPack->breakingBloc);
    destroyTexture(texPack->playerHearth);
    destroyTexture(texPack->itemSlot);
    destroyTexture(texPack->itemAtlas);
    free(texPack);
}
