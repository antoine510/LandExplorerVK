#pragma once

#include <vulkan/vulkan.hpp>
#include "Image.h"
#include "RenderPass.h"
#include "utility/vector.h"

class Swapchain {
public:
	Swapchain(vk::Extent2D extent, vk::PresentModeKHR presentMode);
	Swapchain(const Swapchain& other) = delete;
	Swapchain(Swapchain&& old) = delete;

	~Swapchain();

	void beginCmdBuffer(vk::CommandBuffer& cmdBuffer, Vec4 clearColor);
	void presentCmdBuffer(vk::CommandBuffer& cmdBuffer);

	vk::Extent2D getExtent() const { return _extent; }
	auto& getRenderPass() const { return _renderPass; }

private:
	static vk::Format getColorFormat();

	vk::Format _colorFormat, _depthFormat;

	vk::SwapchainKHR _swapchain;
	vk::Extent2D _extent;
	std::vector<AllocatedImage> _colorImages;
	AllocatedImage _depthImage;

	RenderPass _renderPass;
	std::vector<vk::Framebuffer> _framebuffers;

	vk::Semaphore _hasImage;
	uint32_t _imageIndex;
	vk::SubmitInfo _presentSubmitInfo;
	vk::PresentInfoKHR _presentInfo;
	vk::Fence _imageReady;
};

