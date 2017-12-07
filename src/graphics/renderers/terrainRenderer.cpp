#include "terrainRenderer.h"
#include "graphics/vulkan/Swapchain.h"
#include "utility/mathUtility.h"

void TerrainRenderer::genGrid() {
	std::vector<uint32_t> res;
	res.reserve(6u * gridW * gridH);
	for(uint8_t j = 0; j < gridH; ++j) {
		for(uint8_t i = 0; i < gridW; ++i) {
			uint16_t bloc = i + (j << 8);
			res.push_back(0x0 + bloc); res.push_back(0x20000 + bloc); res.push_back(0x30000 + bloc);
			res.push_back(0x0 + bloc); res.push_back(0x30000 + bloc); res.push_back(0x10000 + bloc);
		}
	}
	_vBuf.getStagingBuffer().update(res.data());
	_vBuf.stageBuffer();
	_vBuf.lock();
}

TerrainRenderer::TerrainRenderer(Swapchain& swapchain, StagedImage& blocAtlas, StagedImage& backwallAtlas, const Vec4& viewOrigin) :
	_blocAtlas(blocAtlas), _backwallAtlas(backwallAtlas), _viewOrigin(viewOrigin),
	_blocBuffer(4 * TERRAIN_WIDTH * TERRAIN_HEIGHT, vk::BufferUsageFlagBits::eStorageBuffer),
	_vBuf(6u * gridW * gridH, 4, vk::Format::eR32Uint),
	_blocSet(_pool, _layout), _backwallSet(_pool, _layout) {
	genGrid();
	_shaders.emplace_back("terrain", vk::ShaderStageFlagBits::eVertex);
	_shaders.emplace_back("terrain", vk::ShaderStageFlagBits::eFragment);

	_blocSet.writeBinding(DescriptorSetBinding(0, vk::DescriptorType::eStorageBufferDynamic, vk::ShaderStageFlagBits::eVertex, _blocBuffer.getBufferDescriptor(0, 4 * gridW * gridH)));
	_blocSet.writeBinding(DescriptorSetBinding(1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, _blocAtlas.getImageInfo()));
	_backwallSet.writeBinding(DescriptorSetBinding(0, vk::DescriptorType::eStorageBufferDynamic, vk::ShaderStageFlagBits::eVertex, _blocBuffer.getBufferDescriptor(0, 4 * gridW * gridH)));
	_backwallSet.writeBinding(DescriptorSetBinding(1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, _backwallAtlas.getImageInfo()));

	_pipeline = new Pipeline(swapchain.getExtent(), _vBuf, vk::PushConstantRange(vk::ShaderStageFlagBits::eAllGraphics, 0u, 32u), _layout, _shaders, swapchain.getRenderPass());
	_pushConsts.pos = glm::vec4(0, 0, 2.0f / viewOrigin.z, 2.0f / viewOrigin.w);
}

vk::DescriptorSetLayout TerrainRenderer::createLayout() {
	std::vector<DescriptorSetBinding> v;
	v.reserve(2);
	v.emplace_back(0, vk::DescriptorType::eStorageBufferDynamic, vk::ShaderStageFlagBits::eVertex);
	v.emplace_back(1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment);
	return DescriptorSet::createLayout(v);
}

void TerrainRenderer::setTerrain(Terrain* terrain) {
	_terrain = terrain;
	updateRect(SDL_Rect{TERRAIN_BORDER, TERRAIN_BORDER, TERRAIN_WIDTH - 2 * TERRAIN_BORDER, TERRAIN_HEIGHT - 2 * TERRAIN_BORDER});
}

void TerrainRenderer::updateRect(SDL_Rect rect) {
	for(int yc = rect.y / gridH; yc < exclusive(float(rect.y + rect.h) / gridH) + 1; ++yc) {
		updateChunck(rect.x / gridW, yc, exclusive(float(rect.x + rect.w) / gridW) + 1 - rect.x / gridW);
	}
}

void TerrainRenderer::updateChunck(int xc, int yc, int wc) {
	std::vector<uint32_t> cdata(gridW * gridH * wc);
	for(int w = 0; w < wc; ++w) {
		for(int y = 0; y < gridH; ++y) {
			for(int x = 0; x < gridW; ++x) {
				Bloc* bloc = getBlockPtr(_terrain, (xc + w) * gridW + x, yc * gridH + y);
				Bloc* backwall = getBackwallPtr(_terrain, (xc + w) * gridW + x, yc * gridH + y);
				// Format: 0x00iiBBbb
				// i = intensity, b = bloc, B = backwall
				cdata[x + (y + w * gridH) * gridW] = _terrain->blocTypes[bloc->type].getProperAtlasOffset(_terrain, bloc)
					+ (_terrain->blocTypes[backwall->type].getProperAtlasOffset(_terrain, backwall) << 8)
					+ ((bloc->light << 4) << 16);
			}
		}
	}
	uint32_t offset = (xc + yc * (TERRAIN_WIDTH / gridW)) * gridW * gridH;
	_blocBuffer.getStagingBuffer().update(cdata.data(), 4 * offset, 4 * cdata.size());
	_blocBuffer.stageBuffer(4 * offset, 4 * cdata.size());
}

void TerrainRenderer::renderTerrain(vk::CommandBuffer& cmdBuf, bool backwall) {
	cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, *_pipeline);
	cmdBuf.bindVertexBuffers(0, (vk::Buffer)_vBuf, (vk::DeviceSize)0);

	_pushConsts.backwall = backwall;
	int xc = int(_viewOrigin.x / gridW),
		yc = int(_viewOrigin.y / gridH),
		xc2 = int((_viewOrigin.x + _viewOrigin.z) / gridW) + 1,
		yc2 = int((_viewOrigin.y + _viewOrigin.w) / gridH) + 1;
	for(int y = yc; y < yc2; ++y) {
		for(int x = xc; x < xc2; ++x) {
			int offset = (x + y * (TERRAIN_WIDTH / gridW)) * gridW * gridH;
			_pushConsts.pos.x = x * gridW - _viewOrigin.x;
			_pushConsts.pos.y = y * gridH - _viewOrigin.y;
			cmdBuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipeline->getLayout(), 0, (vk::DescriptorSet)(backwall ? _backwallSet : _blocSet), 4 * offset);
			cmdBuf.pushConstants<PushConstants>(_pipeline->getLayout(), vk::ShaderStageFlagBits::eAllGraphics, 0, _pushConsts);
			cmdBuf.draw(6 * gridW * gridH, 1, 0, 0);
		}
	}
}
