#include "backgroundRenderer.h"
#include "utility/mathUtility.h"
#include "terrain/terrainGeneration.h"
#include "graphics/graphics.h"
#include "luaScript.h"

static void renderSun(BackgroundRenderer* bgRenderer, vk::CommandBuffer& cmdBuf, float time);

BackgroundRenderer* createBackgroundRenderer() {
	BackgroundRenderer* bgRenderer = (BackgroundRenderer*)calloc(1, sizeof(BackgroundRenderer));

	return bgRenderer;
}

void backgroundRendererLoadTextures(BackgroundRenderer* bgRenderer, LuaScript& script) {
	bgRenderer->sun = script.get<Sprite*>("sun");
	auto bgsscope = script.getScope("backgrounds");
	bgRenderer->background[BG_BIOMES + BIOME_PLAINS] = script.get<Sprite*>("plain");
	bgRenderer->background[BG_BIOMES + BIOME_MOUNTAINS] = script.get<Sprite*>("mountain");
	bgRenderer->background[BG_BIOMES + BIOME_OCEAN] = script.get<Sprite*>("ocean");
	bgRenderer->background[BG_BIOMES + BIOME_UNDERGROUND] = script.get<Sprite*>("underground");
	bgRenderer->background[BG_MENU] = script.get<Sprite*>("menu");
	bgRenderer->background[BG_CREATION] = script.get<Sprite*>("generation");
	bgRenderer->background[BG_WIN] = script.get<Sprite*>("win");
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
		bg->setColorMod(gfx->texPack->skyColor).setPosition(-gfx->viewOrigin.x * 8.0f, offset - gfx->viewOrigin.y * 5.f);
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
