#include "backgroundRenderer.h"
#include "utility/mathUtility.h"
#include "terrain/terrainGeneration.h"
#include "graphics/graphics.h"

static void renderSun(BackgroundRenderer* bgRenderer, vk::CommandBuffer& cmdBuf, float time);

BackgroundRenderer* createBackgroundRenderer() {
	BackgroundRenderer* bgRenderer = (BackgroundRenderer*)calloc(1, sizeof(BackgroundRenderer));

	return bgRenderer;
}

void backgroundRendererLoadTextures(BackgroundRenderer* bgRenderer, xmlNodePtr mainNode) {
	xmlNodePtr texture = mainNode->children;
	while(texture->type == XML_TEXT_NODE) texture = texture->next;

	while(texture) {
		std::string filename("textures/");
		filename += asStringl(mainNode, "path");
		filename += asStringl(texture, "path");
		if(checkName(texture, "background")) {
			int index = asIntl(texture, "id");
			bgRenderer->background[index] = new Sprite(filename);
			auto& sprite = *bgRenderer->background[index];
			if(index == BG_BIOMES + BIOME_PLAINS || index == BG_BIOMES + BIOME_MOUNTAINS || index == BG_BIOMES + BIOME_OCEAN) {
				sprite.setClipSize(TERRAIN_WIDTH * BLOC_SIZE / 2, sprite.getExtent().height).setScale(2.5f).setSampling(true);
			} else {
				sprite.setFullscreen();
			}
		} else if(checkName(texture, "sun")) {
			bgRenderer->sun = new Sprite(filename);
		}
		do texture = texture->next; while(texture && texture->type == XML_TEXT_NODE);
	}
}

void setBackgroundRendererTime(BackgroundRenderer* bgRenderer, float* levelTime) {
	bgRenderer->levelTime = levelTime;
}

void renderBackground(BackgroundRenderer* bgRenderer, Graphics* gfx, int bgID) {
	Sprite* bg = bgRenderer->background[bgID];
	//bg->setColorMod(gfx->texPack->skyColor);
	//SDL_Color bgColor = modulateColor(&gfx->texPack->skyColor, 0.77f, 0.9f, 1.0f);

	int offset = 0;
	switch(bgID) {
	case BG_BIOMES + BIOME_PLAINS: offset = -45; break;
	case BG_BIOMES + BIOME_MOUNTAINS: offset = -25; break;
	case BG_BIOMES + BIOME_OCEAN: offset = -49; break;
	}

	if(offset != 0) {
		renderSun(bgRenderer, gfx->cmdBuf, *bgRenderer->levelTime);
		bg->setPosition(0, offset);
	}
	bg->draw(gfx->cmdBuf);
}

void renderSun(BackgroundRenderer* bgRenderer, vk::CommandBuffer& cmdBuf, float time) {
	if(time > 0.0 && time < 720.0) {
		int x = (int)((0.75f - cosf(time / 1440 * 2 * Constant::pi)) * myDisplayMode.w * 2 / 3);
		int y = (int)((1 - sinf(time / 1440 * 2 * Constant::pi)) * myDisplayMode.h / 2);
		bgRenderer->sun->setPosition(x, y);
		bgRenderer->sun->draw(cmdBuf);
	}
}

void destroyBackgroundRenderer(BackgroundRenderer* bgRenderer) {
	for(Sprite* bg : bgRenderer->background) delete bg;
	delete bgRenderer->sun;
	free(bgRenderer);
}
