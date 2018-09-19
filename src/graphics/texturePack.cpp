#include "texturePack.h"
#include "graphics.h"
#include "luaScript.h"
#include "terrain/blockTypes.h"
#include <SDL_image.h>

TexturePack* initTexturePack(Graphics* gfx) {
	TexturePack* texPack = (TexturePack*)calloc(1, sizeof(TexturePack));

	int texSetCount = 0, buttonSetCount = 0;

	LuaScript texturesLua("textures/textures.lua", {{"TERRAIN_WIDTH", TERRAIN_WIDTH}});

	texPack->blocAtlas = new DeviceImage(texturesLua.get<std::string>("blocAtlas"));
	texPack->backwallAtlas = new DeviceImage(texturesLua.get<std::string>("backwallAtlas"));
	texPack->breakingBloc = texturesLua.get<Sprite*>("breakingBloc");
	texPack->playerHearth = texturesLua.get<Sprite*>("playerHearth");
	texPack->itemSlot = texturesLua.get<Sprite*>("itemSlot");
	texPack->itemAtlas = texturesLua.get<Sprite*>("itemAtlas");

	{
		auto scope(texturesLua.getScope("backgroundRenderer"));
		backgroundRendererLoadTextures(gfx->bgRenderer, texturesLua);
	}
	{
		auto scope(texturesLua.getScope("menuRenderer"));
		menuRendererLoadTextures(&gfx->menuRenderer, texturesLua);
	}
	{
		auto scope(texturesLua.getScope("texSets"));
		int texSetCount = texturesLua.getLength();
		if(texSetCount >= MAX_TEXTURESET_COUNT) throw std::runtime_error("Error while assigning textureSet, please increase MAX_TEXTURESET_COUNT");
		for(int i = 0; i < texSetCount; ++i) {
			auto scope(texturesLua.getScope(i + 1));	// Lua arrays are indexed from 1
			texPack->texSets[i] = createTextureSet(texturesLua);
		}
	}

	return texPack;
}

void blitBreak(TexturePack* texPack, vk::CommandBuffer& cmdBuf, int breakingStep, int x, int y) {
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
	if(gfxData->angle != 0.0f) s->setRotation(gfxData->angle);
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
