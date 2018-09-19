#include "textureSet.h"
#include "luaScript.h"

TextureSet* createTextureSet(LuaScript& script) {
	TextureSet* texSet = (TextureSet*)calloc(1, sizeof(TextureSet));

	int i;
	for(i = 0; i < MAX_ANIMATION_USER; i++) texSet->curAnim[i] = -1;
	texSet->curUser = 0;
	texSet->animated = 0;

	if(script.has("mapIcon")) { setMapIcon(texSet, script.get<Sprite*>("mapIcon")); }
	int texCount = 0;
	if(script.has("states")) {
		auto statescope(script.getScope("states"));
		int stateCount = script.getLength();
		for(int i = 0; i < stateCount; ++i) {
			setTexture(texSet, i, script.get<Sprite*>(i + 1));
			texCount++;
		}
	}
	if(script.has("animations")) {
		auto animsscope(script.getScope("animations"));
		int animCount = script.getLength();
		for(int i = 0; i < animCount; ++i) {
			auto animscope(script.getScope(i + 1));
			Sprite* tex = script.get<Sprite*>();
			int frameWidth = script.get<int>("frameWidth");
			tex->setClipSize(frameWidth, tex->getExtent().height);
			setTexture(texSet, texCount, tex);

			Animation* anim = initAnimation(texCount, tex->getExtent().width, frameWidth, script.get<int>("delay"), script.hasTrue("loop"));
			setAnimation(texSet, anim, i);
			texCount++;
		}
	}

	return texSet;
}

void setTexture(TextureSet* texSet, unsigned int textureID, Sprite* tex) {
	SDL_assert(textureID < MAX_TEXTURE_COUNT);
	texSet->textures[textureID] = tex;
}

void setAnimation(TextureSet* texSet, Animation* animation, unsigned int id) {
	SDL_assert(id < MAX_ANIMATION_COUNT);
	texSet->animations[id] = animation;
	texSet->animated = 1;
}

void setMapIcon(TextureSet* texSet, Sprite* mapIcon) {
	texSet->mapIcon = mapIcon;
}

void setTexSetColorMod(TextureSet* texSet, SDL_Color color) {
	int i;
	for(i = 0; i < MAX_TEXTURE_COUNT; i++) {
		if(texSet->textures[i] != NULL) texSet->textures[i]->setColorMod(color);
	}
}

Sprite* getTexture(TextureSet* texSet, int textureID) {
	if(!texSet->animated) return texSet->textures[textureID];

	if(textureID >= ANIMATION_OFFSET)    //textureID
	{
		int animID = textureID - ANIMATION_OFFSET;
		if(texSet->curAnim[texSet->curUser] != animID && texSet->curAnim[texSet->curUser] != -1)
			reset(texSet->animations[texSet->curAnim[texSet->curUser]], texSet->curUser);
		texSet->curAnim[texSet->curUser] = animID;
		texSet->curUser++;
		texSet->textures[texSet->animations[animID]->textureID]->setClip(getFrameClip(texSet->animations[animID], texSet->curUser - 1), 0);
		return texSet->textures[texSet->animations[animID]->textureID];
	} else        //Animation
	{
		if(texSet->curAnim[texSet->curUser] != -1) {
			reset(texSet->animations[texSet->curAnim[texSet->curUser]], texSet->curUser);
			texSet->curAnim[texSet->curUser] = -1;
		}
		texSet->curUser++;
		return texSet->textures[textureID];
	}
}

void texSetFrameReset(TextureSet* texSet) {
	texSet->curUser = 0;
}

void destroyTextureSet(TextureSet* texSet) {
	int i;
	for(i = 0; i < MAX_TEXTURE_COUNT; i++) {
		delete texSet->textures[i];
	}
	for(i = 0; i < MAX_ANIMATION_COUNT; i++) {
		if(texSet->animations[i] != nullptr) destroyAnimation(texSet->animations[i]);
	}
	delete texSet->mapIcon;
	free(texSet);
}
