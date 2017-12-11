#include "sprite.h"
#include <SDL.h>

#include "vulkan/Shader.h"
#include "vulkan/Swapchain.h"
#include "utility/mathUtility.h"

SpriteRenderer::SpriteRenderer(const Swapchain& swapchain) {
	float spriteVertices[12] = {0, 0,  0, 1,  1, 1,  0, 0,  1, 1,  1, 0};
	_shaders.emplace_back("sprite", vk::ShaderStageFlagBits::eVertex);
	_shaders.emplace_back("sprite", vk::ShaderStageFlagBits::eFragment);
	_vBuf = new VertexBuffer(6, 8, vk::Format::eR32G32Sfloat);
	_vBuf->getStagingBuffer().update(spriteVertices);
	_vBuf->stageBuffer();
	_descPool = DescriptorSet::createPool({{vk::DescriptorType::eCombinedImageSampler, 1}}, maxSprites);
	_descLayout = DescriptorSet::createLayout(DescriptorSetBinding(0, vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment));
	_descSets.reserve(maxSprites);
	for(int i = 0; i < maxSprites; ++i) _descSets.emplace_back(_descPool, _descLayout);
	_pipeline = new Pipeline(swapchain.getExtent(), *_vBuf, vk::PushConstantRange(vk::ShaderStageFlagBits::eAllGraphics, 0u, 60u), _descLayout, _shaders, swapchain.getRenderPass());
}

SpriteRenderer::~SpriteRenderer() {
	delete _pipeline;
	DescriptorSet::destroyPool(_descPool);
	DescriptorSet::destroyLayout(_descLayout);
	delete _vBuf;
	_shaders.clear();
}

