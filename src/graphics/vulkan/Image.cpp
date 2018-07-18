#include "Image.h"
#include <SDL.h>
#include <SDL_image.h>
#include "VulkanCommon.h"
#include "Buffer.h"
#include "CommandBuffer.h"

ImageSamplers::ImageSampler::ImageSampler(vk::Filter magFilter) :
	vk::Sampler(VulkanState::device.createSampler(vk::SamplerCreateInfo().setMinFilter(vk::Filter::eLinear).setMagFilter(magFilter).setMaxAnisotropy(1))) {}

ImageSamplers::ImageSampler::~ImageSampler() {
	VulkanState::device.destroySampler(*this);
}

DeviceImage::DeviceImage(const vk::ImageCreateInfo& imageCI, uint32_t pitch, vk::ImageAspectFlags aspect) :
	_image(VulkanState::device.createImage(imageCI)),
	_layout(imageCI.initialLayout),
	_format(imageCI.format),
	_extent(imageCI.extent),
	_wPitch(pitch ? pitch >> 2 : _extent.width) {
	auto memReqs = VulkanState::device.getImageMemoryRequirements(_image);
	uint32_t textureMemoryType = getMemoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	auto memoryAI = vk::MemoryAllocateInfo(memReqs.size, textureMemoryType);

	_memory = VulkanState::device.allocateMemory(memoryAI);

	VulkanState::device.bindImageMemory(_image, _memory, 0);
	_view = createView(aspect);
}

DeviceImage::DeviceImage(unsigned int width, unsigned int height, unsigned int pitch, vk::Format format) :
	DeviceImage(vk::ImageCreateInfo(vk::ImageCreateFlags(), vk::ImageType::e2D, format, vk::Extent3D(width, height, 1),
									   1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
									   vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled), pitch) {}

DeviceImage::DeviceImage(SDL_Surface* surface) : DeviceImage(surface->w, surface->h, surface->pitch, getSurfaceFormat(surface->format)) {
	SDL_LockSurface(surface);
	uploadPixels(surface->pixels);
	SDL_UnlockSurface(surface);
	SDL_FreeSurface(surface);
}

DeviceImage::~DeviceImage() {
	VulkanState::device.destroyImageView(_view);

	if(!_memory) return;	// Not ours to clean
	VulkanState::device.destroyImage(_image);
	VulkanState::device.freeMemory(_memory);
}

void DeviceImage::switchLayout(vk::ImageLayout newLayout, vk::CommandBuffer& cmdBuffer) {
	auto memoryBarrier = vk::ImageMemoryBarrier()
		.setOldLayout(_layout).setNewLayout(newLayout)
		.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED).setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setImage(_image)
		.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

	vk::PipelineStageFlags sourceStage, dstStage;
	if(_layout == vk::ImageLayout::eShaderReadOnlyOptimal && newLayout == vk::ImageLayout::eTransferDstOptimal) {
		memoryBarrier.setSrcAccessMask(vk::AccessFlagBits::eShaderRead);
		memoryBarrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
		sourceStage = vk::PipelineStageFlagBits::eFragmentShader;
		dstStage = vk::PipelineStageFlagBits::eTransfer;
	} else if(newLayout == vk::ImageLayout::eTransferDstOptimal) {
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

SDL_Surface* DeviceImage::loadSurface(const std::string& path) {
	auto srf = IMG_Load(path.c_str());
	SDL_assert_always(srf != nullptr);
	return srf;
}

void DeviceImage::stageBuffer(const HostBuffer& buf) {
	OneUseCommandBuffer oucb;
	switchLayout(vk::ImageLayout::eTransferDstOptimal, oucb);
	auto region = vk::BufferImageCopy(0, _wPitch, _extent.height, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1)).setImageExtent(_extent);
	oucb.copyBufferToImage(buf, _image, vk::ImageLayout::eTransferDstOptimal, region);
	switchLayout(vk::ImageLayout::eShaderReadOnlyOptimal, oucb);
}

vk::ImageView DeviceImage::createView(vk::ImageAspectFlags aspect) {
	return VulkanState::device.createImageView(vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(), _image, vk::ImageViewType::e2D, _format)
											   .setSubresourceRange(vk::ImageSubresourceRange(aspect, 0, 1, 0, 1)));
}

vk::Format DeviceImage::getSurfaceFormat(SDL_PixelFormat* sfmt) {
	SDL_assert(sfmt->BitsPerPixel == 32);
	return (sfmt->Rmask == 0xff) ? vk::Format::eR8G8B8A8Unorm : vk::Format::eB8G8R8A8Unorm;
}

