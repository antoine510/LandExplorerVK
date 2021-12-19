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

void renderBackground(BackgroundRenderer* bgRenderer, Graphics* gfx, int bgID) {
	Sprite* bg = bgRenderer->background[bgID];

	int offset = 0;
	switch(bgID) {
	case BG_BIOMES + BIOME_PLAINS: offset = 2000; break;
	case BG_BIOMES + BIOME_MOUNTAINS: offset = 1900; break;
	case BG_BIOMES + BIOME_OCEAN: offset = 2250; break;
	}

	if(offset != 0) {
		renderSun(bgRenderer, gfx->cmdBuf, *bgRenderer->levelTime);
		bg->setColorMod(gfx->texPack->skyColor).setPosition(-(int)(gfx->viewOrigin.x * 8.0f), offset - (int)(gfx->viewOrigin.y * 5.f));
	}
	bg->draw(gfx->cmdBuf);
}

void renderSun(BackgroundRenderer* bgRenderer, vk::CommandBuffer& cmdBuf, float time) {
	if(time > 0.0 && time < 720.0) {
		float x = (0.75f - cosf(time / 1440 * 2 * Constant::pi)) * myDisplayMode.w * 2 / 3;
		float y = (1 - sinf(time / 1440 * 2 * Constant::pi)) * myDisplayMode.h / 2;
		bgRenderer->sun->setPosition(x, y);
		bgRenderer->sun->draw(cmdBuf);
	}
}

void destroyBackgroundRenderer(BackgroundRenderer* bgRenderer) {
	for(Sprite* bg : bgRenderer->background) delete bg;
	delete bgRenderer->sun;
	free(bgRenderer);
}
