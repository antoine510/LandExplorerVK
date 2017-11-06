#pragma once

#include "../common.h"
#include "texturePack.h"
#include "chunkRenderer.h"
#include "../level.h"
#include "../entities.h"
#include "renderers/editorRenderer.h"
#include "renderers/mapRenderer.h"
#include "renderers/menuRenderer.h"
#include "renderers/backgroundRenderer.h"
#include "renderers/creditsRenderer.h"
#include "renderers/playerInterfaceRenderer.h"
#include "displayInfo.h"

#include "vulkan/Swapchain.h"
#include "vulkan/RenderPass.h"
#include "vulkan/DescriptorSet.h"
#include "vulkan/Shader.h"
#include "vulkan/Pipeline.h"

struct Graphics {
    SDL_Window* window;
	Swapchain* swapchain;
	DescriptorSet* terrainDescSet;
	Shader* terrainVert, terrainFrag;

	vk::CommandBuffer cmdBuf;

    TexturePack* texPack;
    EditorRenderer editorRenderer;
    MapRenderer mapRenderer;
    MenuRenderer menuRenderer;
    BackgroundRenderer* bgRenderer;
    CreditsRenderer* creditsRenderer;
    PlayerInterfaceRenderer* playerInterfaceRenderer;

    SDL_Point viewOrigin;
    SDL_Rect* playerRect;
    Sprite* deathTextTexture;
};

Graphics* initGraphics();
void setDisplaySize(Graphics* gfx, int w, int h);
void toggleDisplayFullscreen(Graphics* gfx);
void setDisplayFullscreen(Graphics* gfx, bool fullscreen);

void startFrame(Graphics* gfx) { gfx->swapchain->beginCmdBuffer(gfx->cmdBuf); }
void presentFrame(Graphics* gfx) { gfx->swapchain->presentCmdBuffer(gfx->cmdBuf); }

void renderLevel(Graphics* gfx, Level* level);

void initCamera(Graphics* gfx, Entities* entities);
void updateCamera(Graphics* gfx);

void destroyGraphics(Graphics* gfx);

