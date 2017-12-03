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
	AllocatedImage(AllocatedImage&& old) noexcept : _image(old._image), _format(old._format), _memory(old._memory), _layout(old._layout) { old._image = nullptr; old._memory = nullptr; }

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
	ViewedImage(ViewedImage&& old) noexcept : AllocatedImage(std::move(old)), _view(old._view) { old._view = nullptr; }

	~ViewedImage();

	vk::ImageView getView() { return _view; }

protected:
	vk::ImageView _view;
};

class StagedImage : public ViewedImage {
public:
	StagedImage(SDL_Surface* surface, bool isMutable = false);
	StagedImage(const std::string& path, bool isMutable = false);
	StagedImage(unsigned int width, unsigned int height, unsigned int pitch = 0);

	~StagedImage() { _staging.release(); }

	AllocatedBuffer& getStagingBuffer() const { SDL_assert(_staging); return *_staging; }
	void stageImage();

	vk::DescriptorImageInfo getImageInfo() const { return vk::DescriptorImageInfo(_sampler, _view, _layout); }

	void lock() { _staging.release(); }
	void setSampling(bool isLinear) { _sampler = getSampler(isLinear); }

	uint32_t width() const { return _w; }
	uint32_t height() const { return _h; }

protected:
	struct SamplerHelper : vk::Sampler {
		SamplerHelper(vk::Filter maxFilter);
	};
	static SDL_Surface* loadSurface(const std::string& path);
	static vk::Sampler& getSampler(bool isLinear = false);

	uint32_t _w, _h, _wPitch;
	std::unique_ptr<AllocatedBuffer> _staging;

	vk::Sampler& _sampler = getSampler();
};

