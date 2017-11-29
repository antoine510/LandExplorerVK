#include "Image.h"
#include <SDL.h>
#include <SDL_image.h>
#include "VulkanCommon.h"
#include "Buffer.h"
#include "CommandBuffer.h"

AllocatedImage::AllocatedImage(const vk::ImageCreateInfo& imageCI) :
	_image(VulkanState::device.createImage(imageCI)),
	_layout(imageCI.initialLayout),
	_format(imageCI.format) {
	auto memReqs = VulkanState::device.getImageMemoryRequirements(_image);
	uint32_t textureMemoryType = getMemoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	auto memoryAI = vk::MemoryAllocateInfo(memReqs.size, textureMemoryType);

	_memory = VulkanState::device.allocateMemory(memoryAI);

	VulkanState::device.bindImageMemory(_image, _memory, 0);
}

AllocatedImage::~AllocatedImage() {
	if(!_memory) return;
	VulkanState::device.destroyImage(_image);
	VulkanState::device.freeMemory(_memory);
}

void AllocatedImage::switchLayout(vk::ImageLayout newLayout, vk::CommandBuffer& cmdBuffer) {
	auto memoryBarrier = vk::ImageMemoryBarrier()
		.setOldLayout(_layout).setNewLayout(newLayout)
		.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED).setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setImage(_image)
		.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

	vk::PipelineStageFlags sourceStage, dstStage;
	if(_layout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
		memoryBarrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		dstStage = vk::PipelineStageFlagBits::eTransfer;
	} else if(_layout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		memoryBarrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
		memoryBarrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		dstStage = vk::PipelineStageFlagBits::eFragmentShader;
	}

	if(!cmdBuffer) {
		OneUseCommandBuffer oneUseCmdBuffer;
		oneUseCmdBuffer.pipelineBarrier(sourceStage, dstStage, vk::DependencyFlags(), nullptr, nullptr, memoryBarrier);
	} else {
		cmdBuffer.pipelineBarrier(sourceStage, dstStage, vk::DependencyFlags(), nullptr, nullptr, memoryBarrier);
	}
	_layout = newLayout;
}


ViewedImage::ViewedImage(const vk::ImageCreateInfo& imageCI, vk::ImageAspectFlags aspect) : AllocatedImage(imageCI),
_view(VulkanState::device.createImageView(vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(), _image, vk::ImageViewType::e2D, _format)
										  .setSubresourceRange(vk::ImageSubresourceRange(aspect, 0, 1, 0, 1)))) {}

ViewedImage::ViewedImage(const vk::Image& source, vk::Format format, vk::ImageAspectFlags aspect) : AllocatedImage(source, format),
_view(VulkanState::device.createImageView(vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(), _image, vk::ImageViewType::e2D, _format)
										  .setSubresourceRange(vk::ImageSubresourceRange(aspect, 0, 1, 0, 1)))) {}

ViewedImage::~ViewedImage() {
	VulkanState::device.destroyImageView(_view);
}


StagedImage::SamplerHelper::SamplerHelper(vk::Filter maxFilter) :
	vk::Sampler(VulkanState::device.createSampler(vk::SamplerCreateInfo().setMinFilter(vk::Filter::eLinear).setMagFilter(maxFilter))) {}

StagedImage::StagedImage(SDL_Surface* surface, bool isMutable) : StagedImage(surface->w, surface->h, surface->pitch) {
	SDL_LockSurface(surface);
	_staging->update(surface->pixels);
	SDL_UnlockSurface(surface);
	SDL_FreeSurface(surface);
	stageImage();
	if(!isMutable) lock();
}

StagedImage::StagedImage(const std::string& path, bool isMutable) : StagedImage(loadSurface(path), isMutable) {}

StagedImage::StagedImage(unsigned int width, unsigned int height, unsigned int pitch) :
	ViewedImage(vk::ImageCreateInfo(vk::ImageCreateFlags(), vk::ImageType::e2D, vk::Format::eR8G8B8A8Unorm, vk::Extent3D(width, height, 1),
									1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
									vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)),
	_w(width), _h(height), _wPitch(pitch == 0 ? _w : pitch / 4),
	_staging(std::make_unique<AllocatedBuffer>(4 * _wPitch * _h, vk::BufferUsageFlagBits::eTransferSrc,
											   vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)) {}

void StagedImage::stageImage() {
	OneUseCommandBuffer oucb;
	switchLayout(vk::ImageLayout::eTransferDstOptimal, oucb);
	auto region = vk::BufferImageCopy(0, _wPitch, _h, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1))
		.setImageExtent(vk::Extent3D(_w, _h, 1));
	oucb.copyBufferToImage(*_staging, _image, vk::ImageLayout::eTransferDstOptimal, region);
	switchLayout(vk::ImageLayout::eShaderReadOnlyOptimal, oucb);
}

vk::Sampler& StagedImage::getSampler(bool isLinear) {
	static SamplerHelper linearSampler = SamplerHelper(vk::Filter::eLinear), nearestSampler = SamplerHelper(vk::Filter::eNearest);
	return isLinear ? linearSampler : nearestSampler;
}

/*vk::Format StagedImage::getSurfaceFormat(SDL_Surface* srf) {
	format = (srf->format->Rmask == 0x000000ff) ? vk::Format::eR8G8B8A8Unorm : GL_BGRA;
}*/

SDL_Surface* StagedImage::loadSurface(const std::string& path) {
	auto srf = IMG_Load(path.c_str());
	SDL_assert_always(srf != nullptr);
	return srf;
}
