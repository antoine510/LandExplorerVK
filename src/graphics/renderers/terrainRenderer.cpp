#include "terrainRenderer.h"
#include "graphics/vulkan/Swapchain.h"
#include "utility/mathUtility.h"

VertexBuffer genGrid(uint8_t w, uint8_t h) {
	std::vector<uint32_t> res;
	res.reserve(6u * w * h);
	for(uint8_t j = 0; j < h; ++j) {
		for(uint8_t i = 0; i < w; ++i) {
			uint16_t bloc = i + (j << 8);
			res.push_back(0x0 + bloc); res.push_back(0x30000 + bloc); res.push_back(0x10000 + bloc);
			res.push_back(0x0 + bloc); res.push_back(0x20000 + bloc); res.push_back(0x30000 + bloc);
		}
	}
	return VertexBuffer((uint32_t)res.size(), 4, vk::Format::eR32Uint);
}

TerrainRenderer::TerrainRenderer(Swapchain& swapchain, StagedImage& blocAtlas, StagedImage& backwallAtlas, SDL_Point* viewOrigin) :
	_blocAtlas(blocAtlas), _backwallAtlas(backwallAtlas), _viewOrigin(viewOrigin),
	_blocBuffer(4 * gridW * gridH, vk::BufferUsageFlagBits::eStorageBuffer),
	_vBuf(genGrid(gridW, gridH)) {
	_vBuf.lock();
	_shaders.emplace_back("terrain", vk::ShaderStageFlagBits::eVertex);
	_shaders.emplace_back("terrain", vk::ShaderStageFlagBits::eFragment);
	std::vector<DescriptorSetBinding> bindings;
	bindings.emplace_back(0, vk::DescriptorType::eStorageBufferDynamic, vk::ShaderStageFlagBits::eVertex, _blocBuffer.getBufferDescriptor(0, 4 * 64 * 64));
	bindings.emplace_back(1, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, _blocAtlas.getImageInfo());
	bindings.emplace_back(2, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment, _backwallAtlas.getImageInfo());
	_descSet = new DescriptorSet(bindings);
	_pipeline = new Pipeline(swapchain.getExtent(), _vBuf, vk::PushConstantRange(vk::ShaderStageFlagBits::eAllGraphics, 0u, 32u), _descSet->getLayoutRef(), _shaders, swapchain.getRenderPass());
	_pushConsts.pos = glm::vec4(0, 0, 2.0f * BLOC_SIZE / myDisplayMode.w, 2.0f * BLOC_SIZE / myDisplayMode.h);
}

void TerrainRenderer::setTerrain(Terrain* terrain) {
	_terrain = terrain;
	updateRect(SDL_Rect{0, 0, TERRAIN_WIDTH, TERRAIN_HEIGHT});
}

void TerrainRenderer::updateRect(SDL_Rect rect) {
	for(int yc = rect.y / gridH; yc < exclusive(float(rect.y + rect.h) / gridH) + 1; ++yc) {
		updateChunck(rect.x / gridW, yc, exclusive(float(rect.x + rect.w) / gridW) + 1 - rect.x / gridW);
	}
}

void TerrainRenderer::updateChunck(int xc, int yc, int wc) {
	std::vector<uint32_t> cdata(gridW * gridH * wc);
	for(int w = 0; w < wc; ++wc) {
		for(int y = 0; y < gridH; ++y) {
			for(int x = 0; x < gridW; ++x) {
				Bloc* bloc = getBlockPtr(_terrain, (xc + wc) * gridW + x, yc * gridH + y);
				Bloc* backwall = getBackwallPtr(_terrain, (xc + wc) * gridW + x, yc * gridH + y);
				// Format: 0x00iiBBbb
				// i = intensity, b = bloc, B = backwall
				cdata[x + (y + w * gridH) * gridW] = _terrain->blocTypes[bloc->type].getAtlasOffset(_terrain, bloc)
					+ (_terrain->blocTypes[backwall->type].getAtlasOffset(_terrain, backwall) << 8)
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
	int xc = (_viewOrigin->x - myDisplayMode.w / 2) / (gridW * BLOC_SIZE),
		yc = (_viewOrigin->y - myDisplayMode.h / 2) / (gridH * BLOC_SIZE),
		xc2 = (_viewOrigin->x + myDisplayMode.w / 2) / (gridW * BLOC_SIZE) + 1,
		yc2 = (_viewOrigin->y + myDisplayMode.h / 2) / (gridH * BLOC_SIZE) + 1;
	for(int y = yc; y < yc2; ++y) {
		for(int x = xc; x < xc2; ++x) {
			int offset = (x + y * (TERRAIN_WIDTH / gridW)) * gridW * gridH;
			_pushConsts.pos.x = x * gridW - float(_viewOrigin->x) / BLOC_SIZE;
			_pushConsts.pos.y = y * gridH - float(_viewOrigin->y) / BLOC_SIZE;
			cmdBuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _pipeline->getLayout(), 0, (vk::DescriptorSet)*_descSet, 4 * offset);
			cmdBuf.pushConstants<PushConstants>(_pipeline->getLayout(), vk::ShaderStageFlagBits::eAllGraphics, 0, _pushConsts);
			cmdBuf.draw(6 * gridW * gridH, 1, 0, 0);
		}
	}
}
