#include "texturePack.h"
#include "graphics.h"
#include "utility/xmlTools.h"
#include "terrain/blockTypes.h"
#include <SDL_image.h>

TexturePack* initTexturePack(Graphics* gfx) {
	TexturePack* texPack = (TexturePack*)calloc(1, sizeof(TexturePack));

	int texSetCount = 0, buttonSetCount = 0;

	xmlDocPtr texturesDoc = parseXML("textures/textures.xml");
	xmlNodePtr texture = xmlDocGetRootElement(texturesDoc)->xmlChildrenNode;
	while(texture->type == XML_TEXT_NODE) texture = texture->next;

	while(texture) {
		std::string filename("textures/");
		filename += asStringl(texture, "path");
		if(checkName(texture, "blocAtlas")) {
			texPack->blocAtlas = new StagedImage(filename);
		} else if(checkName(texture, "backwallAtlas")) {
			texPack->backwallAtlas = new StagedImage(filename);
		} else if(checkName(texture, "breakingBloc")) {
			texPack->breakingBloc = new Sprite(filename);
			texPack->breakingBloc->setClipSize(BLOC_SIZE, BLOC_SIZE);
		} else if(checkName(texture, "playerHearth")) {
			texPack->playerHearth = new Sprite(filename);
			texPack->playerHearth->setScreenOrigin(1, 0);
		} else if(checkName(texture, "itemSlot")) {
			texPack->itemSlot = new Sprite(filename);
		} else if(checkName(texture, "itemAtlas")) {
			texPack->itemAtlas = new Sprite(filename);
			texPack->itemAtlas->setClipSize(TEXTURE_ITEM_SIZE, TEXTURE_ITEM_SIZE);
		} else if(checkName(texture, "backgroundRenderer")) {
			backgroundRendererLoadTextures(gfx->bgRenderer, texture);
		} else if(checkName(texture, "menuRenderer")) {
			menuRendererLoadTextures(&gfx->menuRenderer, texture);
		} else if(checkName(texture, "texSet")) {
			if(texSetCount >= MAX_TEXTURESET_COUNT) printf("Error while assigning textureSet, please increase MAX_TEXTURESET_COUNT");
			texPack->texSets[texSetCount] = createTextureSet(texture, filename.c_str());
			texSetCount++;
		} else if(checkName(texture, "buttonSet")) {
			if(buttonSetCount >= MAX_BUTTONSET_COUNT) printf("Error while assigning buttonSet, please increase MAX_BUTTONSET_COUNT");
			texPack->buttonSets[buttonSetCount] = createTextureSet(texture, filename.c_str());
			buttonSetCount++;
		}

		do texture = texture->next; while(texture && texture->type == XML_TEXT_NODE);
	}
	xmlFreeDoc(texturesDoc);

	return texPack;
}

void blitBreak(TexturePack* texPack, vk::CommandBuffer& cmdBuf, int breakingStep, Uint32 x, Uint32 y) {
	texPack->breakingBloc->setClip(breakingStep, 0);
	texPack->breakingBloc->setPosition(x, y);
	texPack->breakingBloc->draw(cmdBuf);
}

void blitMapIcon(TexturePack* texPack, vk::CommandBuffer& cmdBuf, GfxData* gfxData, SDL_Rect rect, float scalingFactor, SDL_Point panningPos) {
	texPack->texSets[gfxData->texID]->mapIcon->setPosition(
		(int)((rect.x + rect.w / 2) * scalingFactor / BLOC_SIZE) - panningPos.x,
		(int)((rect.y + rect.h / 2) * scalingFactor / BLOC_SIZE) - panningPos.y);
	texPack->texSets[gfxData->texID]->mapIcon->draw(cmdBuf);
}

void blitEntity(TexturePack* texPack, vk::CommandBuffer& cmdBuf, GfxData* gfxData, SDL_Rect rect) {
	Sprite* s = getTexture(texPack->texSets[gfxData->texID], gfxData->texState);
	s->setPosition(rect.x, rect.y).setColorMod(texPack->skyColor);
	//setTextureAngle(tex, gfxData->angle);
	s->draw(cmdBuf);
}

/** \brief Draws a hearth on screen
 *
 * \param texPack The texture pack which contains the hearth texture
 * \param rect The drawing position
 * \param ratioFull The ratio to a full hearth
 */
void blitHearth(TexturePack* texPack, vk::CommandBuffer& cmdBuf, SDL_Point pos, float ratioFull) {
	texPack->playerHearth->setAlphaMod(ratioFull);
	texPack->playerHearth->setPosition(pos.x, pos.y);
	texPack->playerHearth->draw(cmdBuf);
}

/** \brief Draws an item slot on screen
 *
 * \param texPack The texture pack which contains the item slot texture
 * \param rect The drawing position
 */
void blitSlot(TexturePack* texPack, vk::CommandBuffer& cmdBuf, SDL_Point pos, float sizeMul) {
	texPack->itemSlot->setScale(sizeMul);
	texPack->itemSlot->setPosition(pos.x, pos.y);
	texPack->itemSlot->draw(cmdBuf);
}

/** \brief Draws an item on screen
 *
 * \param texPack The texture pack which contains the item texture
 * \param rect The drawing position
 * \param itemID The item ID
 */
void blitItem(TexturePack* texPack, vk::CommandBuffer& cmdBuf, SDL_Point pos, int itemID) {
	texPack->itemAtlas->setClip(itemID % 32, itemID / 32);
	texPack->itemAtlas->setPosition(pos.x, pos.y);
	texPack->itemAtlas->draw(cmdBuf);
}

/** \brief Resets all texture set current users
 *
 * \param texPack The texture pack which contains the texture sets
 */
void texPackFrameReset(TexturePack* texPack) {
	int i;
	for(i = 0; i < MAX_TEXTURESET_COUNT; i++) {
		if(texPack->texSets[i] != NULL) texSetFrameReset(texPack->texSets[i]);
	}
}

/** \brief Destroys a texture pack
 *
 * \param texPack The texture pack to be destroyed
 */
void destroyTexturePack(TexturePack* texPack) {
	int i;
	for(i = 0; i < MAX_TEXTURESET_COUNT; i++) {
		if(texPack->texSets[i] != nullptr) destroyTextureSet(texPack->texSets[i]);
	}
	for(i = 0; i < MAX_BUTTONSET_COUNT; i++) {
		if(texPack->buttonSets[i] != nullptr) destroyTextureSet(texPack->buttonSets[i]);
	}

	delete texPack->blocAtlas;
	delete texPack->backwallAtlas;

	delete texPack->breakingBloc;
	delete texPack->playerHearth;
	delete texPack->itemSlot;
	delete texPack->itemAtlas;
	free(texPack);
}
