#pragma once

#include <vulkan/vulkan.hpp>
#include <SDL_assert.h>
#include <unordered_map>
#include "Buffer.h"

struct SDL_Surface;

class AllocatedImage {
public:
	AllocatedImage(const vk::ImageCreateInfo& imageCI);
	AllocatedImage(const vk::Image& source, vk::Format format) : _image(source), _format(format) {}

	~AllocatedImage();

	void switchLayout(vk::ImageLayout newLayout, vk::CommandBuffer& cmdBuffer = vk::CommandBuffer());

protected:
	vk::Image _image;
	vk::Format _format;
	vk::DeviceMemory _memory;
	vk::ImageLayout _layout = vk::ImageLayout::eUndefined;
};

class ViewedImage : public AllocatedImage {
public:
	ViewedImage(const vk::ImageCreateInfo& imageCI, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor);
	ViewedImage(const vk::Image& source, vk::Format format, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor);
	~ViewedImage();

	vk::ImageView& getView() { return _view; }

protected:
	vk::ImageView _view;

};

class StagedImage : public ViewedImage {
public:
	StagedImage(SDL_Surface* surface);
	StagedImage(const std::string& path);
	StagedImage(unsigned int width, unsigned int height, unsigned int pitch = 0);
	~StagedImage() { _staging.release(); }

	AllocatedBuffer& getStagingBuffer() { SDL_assert(_staging); return *_staging; }
	void stageImage();

	vk::DescriptorImageInfo getImageInfo() { return vk::DescriptorImageInfo(_sampler, _view, _layout); }

	void lock() { _staging.release(); }
	void setSampling(bool linear) { _sampler = getSampler(linear); }

	uint32_t width() const { return _w; }
	uint32_t height() const { return _h; }

protected:
	struct SamplerHelper : vk::Sampler {
		SamplerHelper(vk::Filter maxFilter);
	};
	static SDL_Surface* loadSurface(const std::string& path);
	static vk::Sampler& getSampler(bool linear = false);

	std::unique_ptr<AllocatedBuffer> _staging;
	uint32_t _w, _h, _wPitch;

	vk::Sampler& _sampler = getSampler();
};

