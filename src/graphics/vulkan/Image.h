#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL_assert.h>
#include <unordered_map>
#include "Buffer.h"

struct SDL_Surface;

class AllocatedImage {
public:
	AllocatedImage(const vk::ImageCreateInfo& imageCI, uint32_t pitch = 0, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor);
	AllocatedImage(const vk::Image& source, vk::Format format, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor) :
		_image(source), _format(format), _view(createView(aspect)) {}
	AllocatedImage(AllocatedImage&& old) noexcept :
		_image(old._image), _format(old._format), _memory(old._memory), _layout(old._layout), _view(old._view), _extent(old._extent), _wPitch(old._wPitch) {
		old._image = nullptr; old._memory = nullptr; old._view = nullptr;
	}

	~AllocatedImage();

	void switchLayout(vk::ImageLayout newLayout, vk::CommandBuffer& cmdBuffer = vk::CommandBuffer());

	const vk::Extent3D getExtent() const { return _extent; }
	vk::ImageView getView() { return _view; }

protected:
	vk::ImageView createView(vk::ImageAspectFlags aspect);

	vk::Image _image;
	vk::Format _format;
	vk::DeviceMemory _memory;
	vk::ImageLayout _layout = vk::ImageLayout::eUndefined;

	vk::ImageView _view;

	vk::Extent3D _extent;
	uint32_t _wPitch;
};

struct ImageSamplers {
	ImageSamplers() : linearSampler(vk::Filter::eLinear), nearestSampler(vk::Filter::eNearest) {}

	struct ImageSampler : vk::Sampler {
		ImageSampler(vk::Filter maxFilter);
		~ImageSampler();
	};

	ImageSampler linearSampler, nearestSampler;
};
extern ImageSamplers* imageSamplers;

class SampledImage : public AllocatedImage {
public:
	SampledImage(SDL_Surface* surface);
	SampledImage(const std::string& path) : SampledImage(loadSurface(path)) {}
	SampledImage(unsigned int width, unsigned int height, unsigned int pitch = 0);

	~SampledImage() {}

	vk::DescriptorImageInfo getImageInfo() const { return vk::DescriptorImageInfo(_sampler, _view, _layout); }

	void setSampling(bool isLinear) { _sampler = isLinear ? imageSamplers->linearSampler : imageSamplers->nearestSampler; }

	void stageBuffer(const AllocatedBuffer& buf);

protected:
	static SDL_Surface* loadSurface(const std::string& path);
	AllocatedBuffer createStagingBuffer() const;
	void uploadPixels(const void* pixels);

	vk::Sampler _sampler = imageSamplers->nearestSampler;
};

class StagedImage : public SampledImage {
public:
	StagedImage(SDL_Surface* surface) : SampledImage(surface) {}
	StagedImage(const std::string& path) : SampledImage(loadSurface(path)) {}
	StagedImage(unsigned int width, unsigned int height, unsigned int pitch = 0) : SampledImage(width, height, pitch) {}

	AllocatedBuffer& getStagingBuffer() { return _staging; }
	void stageImage() { stageBuffer(_staging); }

protected:
	void uploadPixels(const void* pixels) { _staging.update(pixels); stageImage(); }

	AllocatedBuffer _staging = createStagingBuffer();
};

