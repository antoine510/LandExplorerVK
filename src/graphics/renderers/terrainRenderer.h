#pragma once

#include "common.h"
#include "map.h"
#include "level.h"
#include "graphics/vulkan/Shader.h"
#include "graphics/sprite.h"

class TerrainRenderer {
public:
	TerrainRenderer(Swapchain& swapchain, StagedImage& blocAtlas, StagedImage& backwallAtlas, SDL_Point* viewOrigin);
	~TerrainRenderer() { delete _descSet; delete _pipeline; }

	void setTerrain(Terrain* terrain);
	void setSkyColor(SDL_Color color) {
		_pushConsts.skyColor.r = color.r / 255.0f;
		_pushConsts.skyColor.g = color.g / 255.0f;
		_pushConsts.skyColor.b = color.b / 255.0f;
	}

	void updateRect(SDL_Rect rect);
	void updatePos(int x, int y) { updateRect(SDL_Rect{x, y, 1, 1}); }

	void renderTerrain(vk::CommandBuffer& cmdBuf, bool backwall);

private:
	static constexpr int gridW = 16, gridH = 16;

	struct PushConstants {
		glm::vec4 pos;
		glm::vec3 skyColor;
		vk::Bool32 backwall;
	};

	void updateChunck(int xc, int yc, int wc);

	VertexBuffer _vBuf;
	DescriptorSet* _descSet;
	Pipeline* _pipeline;
	std::vector<Shader> _shaders;

	const StagedImage& _blocAtlas, &_backwallAtlas;
	Terrain* _terrain;
	SDL_Point* _viewOrigin;

	PushConstants _pushConsts;
	StagedBuffer _blocBuffer;
};

