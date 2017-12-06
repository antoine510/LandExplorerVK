#pragma once

#include "common.h"
#include "map.h"
#include "level.h"
#include "graphics/vulkan/Shader.h"
#include "graphics/sprite.h"

class TerrainRenderer {
public:
	TerrainRenderer(Swapchain& swapchain, StagedImage& blocAtlas, StagedImage& backwallAtlas, const Vec4& viewOrigin);
	~TerrainRenderer() { DescriptorSet::destroyPool(_pool); DescriptorSet::destroyLayout(_layout); delete _pipeline; }

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
	static constexpr int gridW = 16, gridH = 16; // ! Shader depends on these values !

	struct PushConstants {
		glm::vec4 pos;
		glm::vec3 skyColor = glm::vec3(1, 1, 1);
		vk::Bool32 backwall;
	};

	void genGrid();
	void updateChunck(int xc, int yc, int wc);

	vk::DescriptorSetLayout createLayout();

	VertexBuffer _vBuf;
	vk::DescriptorPool _pool = DescriptorSet::createPool({{vk::DescriptorType::eStorageBufferDynamic, 1}, {vk::DescriptorType::eCombinedImageSampler, 1}}, 2);
	vk::DescriptorSetLayout _layout = createLayout();
	DescriptorSet _blocSet, _backwallSet;
	Pipeline* _pipeline;
	std::vector<Shader> _shaders;

	const StagedImage& _blocAtlas, &_backwallAtlas;
	Terrain* _terrain;
	const Vec4& _viewOrigin;

	PushConstants _pushConsts;
	StagedBuffer _blocBuffer;
};

