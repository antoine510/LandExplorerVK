#pragma once

#include "../common.h"
#include "texturePack.h"
#include "../level.h"
#include "../entities.h"
#include "renderers/editorRenderer.h"
#include "renderers/mapRenderer.h"
#include "renderers/menuRenderer.h"
#include "renderers/backgroundRenderer.h"
#include "renderers/creditsRenderer.h"
#include "renderers/playerInterfaceRenderer.h"
#include "renderers/terrainRenderer.h"
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

    Vec4 viewOrigin;
    SDL_Rect* playerRect;
};

Graphics* initGraphics();
void setDisplaySize(Graphics* gfx, int w, int h, bool fullscreen = false);

inline void startFrame(Graphics* gfx) { gfx->swapchain->beginCmdBuffer(gfx->cmdBuf, gfx->texPack->skyColor * Vec4(0.77f, 0.9f, 1.0f, 1.0f)); }
inline void presentFrame(Graphics* gfx) { gfx->swapchain->presentCmdBuffer(gfx->cmdBuf); }

void renderLevel(Graphics* gfx, Level* level);

void initCamera(Graphics* gfx, Entities* entities);
void updateCamera(Graphics* gfx);

void destroyGraphics(Graphics* gfx);

