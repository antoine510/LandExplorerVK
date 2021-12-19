#include "graphics.h"
#include "utility/mathUtility.h"
#include "terrain/blockTypes.h"
#include "vulkan/VulkanCommon.h"

static void renderEntities(Graphics* gfx, Entities* entities);
static void initRenderers(Graphics* gfx);
static void destroyRenderers(Graphics* gfx);

SDL_DisplayMode myDisplayMode;
ImageSamplers* imageSamplers = nullptr;
TerrainRenderer* terrainRenderer = nullptr;
SpriteRenderer* spriteRenderer = nullptr;

Graphics* initGraphics() {
	Graphics* gfx = new Graphics;

	gfx->window = SDL_CreateWindow("Land Explorer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_VULKAN);
	SDL_assert_release(gfx->window != NULL);

	SDL_GetWindowDisplayMode(gfx->window, &myDisplayMode);
	gfx->viewOrigin = Vec4{0, 0, float(myDisplayMode.w) / BLOC_SIZE, float(myDisplayMode.h) / BLOC_SIZE};
	gfx->playerRect = NULL;

	VulkanState::setup(gfx->window);
	imageSamplers = new ImageSamplers();
	gfx->swapchain = new Swapchain(vk::Extent2D(myDisplayMode.w, myDisplayMode.h), vk::PresentModeKHR::eFifo);

	initRenderers(gfx);

	gfx->cmdBuf = VulkanState::device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(VulkanState::cmdPool, vk::CommandBufferLevel::ePrimary, 1u))[0];

	return gfx;
}

void initRenderers(Graphics* gfx) {
	spriteRenderer = new SpriteRenderer(*gfx->swapchain);
	gfx->creditsRenderer = createCreditsRenderer();
	gfx->playerInterfaceRenderer = createPlayerInterfaceRenderer(gfx);
	gfx->bgRenderer = createBackgroundRenderer();
	initMenuRenderer(&gfx->menuRenderer);
	initMapRenderer(&gfx->mapRenderer, gfx);
	gfx->texPack = initTexturePack(gfx);
	terrainRenderer = new TerrainRenderer(*gfx->swapchain, *gfx->texPack->blocAtlas, *gfx->texPack->backwallAtlas, gfx->viewOrigin);
	initEditorRenderer(&gfx->editorRenderer, gfx);
}

void setDisplaySize(Graphics* gfx, int w, int h, bool fullscreen) {
	if(w != myDisplayMode.w || h != myDisplayMode.h) {
		presentFrame(gfx);
		destroyRenderers(gfx);
		delete gfx->swapchain;

		if(fullscreen) {
			SDL_DisplayMode mode;
			SDL_GetDesktopDisplayMode(0, &mode);
			SDL_SetWindowSize(gfx->window, mode.w, mode.h);
			SDL_SetWindowFullscreen(gfx->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		} else {
			SDL_SetWindowFullscreen(gfx->window, 0);
			SDL_SetWindowSize(gfx->window, w, h);
		}
		SDL_GetWindowDisplayMode(gfx->window, &myDisplayMode);

		gfx->viewOrigin = Vec4{0, 0, float(myDisplayMode.w) / BLOC_SIZE, float(myDisplayMode.h) / BLOC_SIZE};
		gfx->swapchain = new Swapchain(vk::Extent2D(myDisplayMode.w, myDisplayMode.h), vk::PresentModeKHR::eFifo);
		initRenderers(gfx);
		startFrame(gfx);
		spriteRenderer->bind(gfx->cmdBuf);
	}
}

void renderLevel(Graphics* gfx, Level* level) {
	gfx->texPack->skyColor = colorToVec(level->skyColor);

	Vec2 playerPos = level->entities->collData[level->playerID].pos;
	int biomeID = getChunckPtr(level->terrain, (Uint32)(playerPos.x / CHUNCK_SIZE), (Uint32)(playerPos.y / CHUNCK_SIZE))->biome;
	renderBackground(gfx->bgRenderer, gfx, BG_BIOMES + biomeID);

	updateCamera(gfx);

	terrainRenderer->setSkyColor(gfx->texPack->skyColor);
	terrainRenderer->renderTerrain(gfx->cmdBuf, true);
	terrainRenderer->renderTerrain(gfx->cmdBuf, false);
	spriteRenderer->bind(gfx->cmdBuf);
	renderEntities(gfx, level->entities);

	renderPlayerInterface(gfx, level->playerControl);
}

void initCamera(Graphics* gfx, Entities* entities) {
	int i;
	Uint32 target = COMP_PLAYER_CONTROL + COMP_RECT;
	for(i = 0; i < ENTITY_COUNT; i++) {
		if((entities->components[i] & target) == target) {
			gfx->playerRect = &entities->rect[i];
			break;
		}
	}
	updateCamera(gfx);

	initMapRendererStartPos(&gfx->mapRenderer, gfx);
}

void updateCamera(Graphics* gfx) {
	//gfx->viewOrigin.x = lerp(gfx->viewOrigin.x, gfx->playerRect->x + gfx->playerRect->w/2 - myDisplayMode->w/2, 0.5f);
	//gfx->viewOrigin.x = lerp(gfx->viewOrigin.y, gfx->playerRect->y + gfx->playerRect->h/2, 0.25f);
	gfx->viewOrigin.x = float(gfx->playerRect->x + gfx->playerRect->w / 2 - myDisplayMode.w / 2) / BLOC_SIZE;
	gfx->viewOrigin.y = float(gfx->playerRect->y + gfx->playerRect->h / 2 - myDisplayMode.h / 2) / BLOC_SIZE;

	//Violent movements should not be interpolated
	//if(abs(gfx->viewOrigin.y - (gfx->playerRect->y + gfx->playerRect->h / 2)) > 2 * BLOC_SIZE)
	//	gfx->viewOrigin.y = gfx->playerRect->y + gfx->playerRect->h / 2;		///ALWAYS USED !!! TODO

	//Border conditions
	gfx->viewOrigin.x = std::clamp(gfx->viewOrigin.x, float(TERRAIN_BORDER), TERRAIN_WIDTH - TERRAIN_BORDER - gfx->viewOrigin.z);
	gfx->viewOrigin.y = std::clamp(gfx->viewOrigin.y, float(TERRAIN_BORDER), TERRAIN_HEIGHT - TERRAIN_BORDER - gfx->viewOrigin.w);
}

void renderEntities(Graphics* gfx, Entities* entities) {
	int i;
	Uint32 target = COMP_GFXDATA + COMP_RECT;
	for(i = 0; i < ENTITY_COUNT; i++) {
		if((entities->components[i] & target) == target) {
			SDL_Rect t = entities->rect[i];
			t.x -= int(gfx->viewOrigin.x * BLOC_SIZE);
			t.y -= int(gfx->viewOrigin.y * BLOC_SIZE);
			blitEntity(gfx->texPack, gfx->cmdBuf, &entities->gfxData[i], t);
		}
	}
	texPackFrameReset(gfx->texPack);
}

void destroyRenderers(Graphics* gfx) {
	destroyEditorRenderer(&gfx->editorRenderer);
	delete terrainRenderer;
	destroyTexturePack(gfx->texPack);
	destroyMapRenderer(&gfx->mapRenderer);
	destroyMenuRenderer(&gfx->menuRenderer);
	destroyBackgroundRenderer(gfx->bgRenderer);
	destroyPlayerInterfaceRenderer(gfx->playerInterfaceRenderer);
	destroyCreditsRenderer(gfx->creditsRenderer);
	delete spriteRenderer;
}

void destroyGraphics(Graphics* gfx) {
	destroyRenderers(gfx);

	delete imageSamplers;
	delete gfx->swapchain;
	VulkanState::teardown();

	SDL_DestroyWindow(gfx->window);
	free(gfx);
}

