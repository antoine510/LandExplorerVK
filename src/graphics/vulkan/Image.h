#pragma once

#include <vulkan/vulkan.hpp>
#include "Buffer.h"

struct SDL_Surface;
struct SDL_PixelFormat;

struct ImageSamplers {
	ImageSamplers() : linearSampler(vk::Filter::eLinear), nearestSampler(vk::Filter::eNearest) {}

	struct ImageSampler : vk::Sampler {
		ImageSampler(vk::Filter magFilter);
		~ImageSampler();
	};

	ImageSampler linearSampler, nearestSampler;
};
extern ImageSamplers* imageSamplers;

class DeviceImage {
public:
	DeviceImage(const vk::ImageCreateInfo& imageCI, uint32_t pitch = 0, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor);
	DeviceImage(const vk::Image& source, vk::Format format, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor) :
		_image(source), _format(format), _view(createView(aspect)) {}
	DeviceImage(DeviceImage&& old) noexcept :
		_image(old._image), _format(old._format), _memory(old._memory), _layout(old._layout), _view(old._view), _extent(old._extent), _wPitch(old._wPitch) {
		old._image = nullptr; old._memory = nullptr; old._view = nullptr;
	}

	DeviceImage(SDL_Surface* surface);
	DeviceImage(const std::string& path) : DeviceImage(loadSurface(path)) {}
	DeviceImage(unsigned int width, unsigned int height, unsigned int pitch = 0, vk::Format format = vk::Format::eB8G8R8A8Unorm);

	virtual ~DeviceImage();

	vk::DescriptorImageInfo getImageInfo() const { return vk::DescriptorImageInfo(_sampler, _view, _layout); }
	void switchLayout(vk::ImageLayout newLayout, vk::CommandBuffer& cmdBuffer = vk::CommandBuffer());
	void setSampling(bool isLinear) { _sampler = isLinear ? imageSamplers->linearSampler : imageSamplers->nearestSampler; }

	void stageBuffer(const HostBuffer& buf);

	const vk::Extent3D getExtent() const { return _extent; }
	vk::ImageView getView() { return _view; }

protected:
	vk::ImageView createView(vk::ImageAspectFlags aspect);
	static vk::Format getSurfaceFormat(SDL_PixelFormat* sfmt);
	static SDL_Surface* loadSurface(const std::string& path);
	void uploadPixels(const void* pixels) { stageBuffer(HostBuffer(pixels, 4 * _wPitch * _extent.height)); }

	vk::Image _image;
	vk::Format _format;
	vk::DeviceMemory _memory;
	vk::ImageLayout _layout = vk::ImageLayout::eUndefined;

	vk::ImageView _view;

	vk::Extent3D _extent;
	uint32_t _wPitch;

	vk::Sampler _sampler = imageSamplers->nearestSampler;
};

