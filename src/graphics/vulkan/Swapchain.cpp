#include "Swapchain.h"
#include <SDL_assert.h>
#include "VulkanCommon.h"

const vk::PipelineStageFlags imageRequiredStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

Swapchain::Swapchain(vk::Extent2D extent, vk::PresentModeKHR presentMode) :
	_colorFormat(getColorFormat()),
	_depthFormat(vk::Format::eD16Unorm),
	_depthImage(vk::ImageCreateInfo(vk::ImageCreateFlags(), vk::ImageType::e2D, _depthFormat, vk::Extent3D(extent.width, extent.height, 1),
									1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment),
				0 ,vk::ImageAspectFlagBits::eDepth),
	_renderPass(_colorFormat, _depthFormat),
	_hasImage(VulkanState::device.createSemaphore(vk::SemaphoreCreateInfo())),
	_presentSubmitInfo(vk::SubmitInfo(1, &_hasImage, &imageRequiredStage, 1)),
	_presentInfo(0, nullptr, 1, &_swapchain, &_imageIndex),
	_imageReady(VulkanState::device.createFence(vk::FenceCreateInfo())),
	_extent(extent) {

	auto windowPresentModes = VulkanState::gpu.getSurfacePresentModesKHR(VulkanState::windowKHR);
	SDL_assert_always(std::find(windowPresentModes.begin(), windowPresentModes.end(), presentMode) != windowPresentModes.end());

	auto swapchainCI = vk::SwapchainCreateInfoKHR()
		.setSurface(VulkanState::windowKHR)
		.setMinImageCount(2)
		.setImageFormat(_colorFormat)
		.setImageExtent(extent)
		.setImageArrayLayers(1)
		.setPresentMode(presentMode)
		.setClipped(true)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

	_swapchain = VulkanState::device.createSwapchainKHR(swapchainCI);
	auto swapchainImages = VulkanState::device.getSwapchainImagesKHR(_swapchain);
	for(const auto& swapchainImage : swapchainImages) {
		_colorImages.emplace_back(swapchainImage, _colorFormat, vk::ImageAspectFlagBits::eColor);
	}

	std::vector<vk::ImageView> attachementViews(2);

	vk::FramebufferCreateInfo framebufferCI(vk::FramebufferCreateFlags(), _renderPass, 2, attachementViews.data(), _extent.width, _extent.height, 1);
	attachementViews[1] = _depthImage.getView();
	for(auto& colorImage : _colorImages) {
		attachementViews[0] = colorImage.getView();
		_framebuffers.push_back(VulkanState::device.createFramebuffer(framebufferCI));
	}
}

Swapchain::~Swapchain() {
	VulkanState::device.waitIdle();

	for(auto& fbuf : _framebuffers) VulkanState::device.destroyFramebuffer(fbuf);
	VulkanState::device.destroySemaphore(_hasImage);
	VulkanState::device.destroyFence(_imageReady);
	VulkanState::device.destroySwapchainKHR(_swapchain);
}

void Swapchain::beginCmdBuffer(const vk::CommandBuffer& cmdBuffer, const Vec4& clearColor) {
	_imageIndex = VulkanState::device.acquireNextImageKHR(_swapchain, UINT64_MAX, _hasImage, vk::Fence()).value;

	vk::ClearValue depthClear[2] = {vk::ClearColorValue(std::array<float, 4>{clearColor.r, clearColor.g, clearColor.b, clearColor.a}), vk::ClearDepthStencilValue(1.f, 0)};
	auto renderPassBeginInfo = vk::RenderPassBeginInfo(_renderPass, _framebuffers[_imageIndex], vk::Rect2D(vk::Offset2D(0, 0), _extent), 2, depthClear);

	cmdBuffer.begin(vk::CommandBufferBeginInfo());
	cmdBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

vk::Format Swapchain::getColorFormat() {
	auto windowFormats = VulkanState::gpu.getSurfaceFormatsKHR(VulkanState::windowKHR);
	if(windowFormats.size() == 1 && windowFormats[0].format == vk::Format::eUndefined) {
		return vk::Format::eB8G8R8A8Unorm;
	} else {
		SDL_assert_always(windowFormats.size() >= 1);
		return windowFormats[0].format;
	}
}

void Swapchain::presentCmdBuffer(const vk::CommandBuffer& cmdBuffer) {
	cmdBuffer.endRenderPass();
	cmdBuffer.end();

	_presentSubmitInfo.setPCommandBuffers(&cmdBuffer);
	VulkanState::deviceQueue.submit(_presentSubmitInfo, _imageReady);


	VulkanState::device.waitForFences(_imageReady, true, UINT64_MAX);	// TODO: fix this stall

	VulkanState::deviceQueue.presentKHR(_presentInfo);
	cmdBuffer.reset(vk::CommandBufferResetFlags());
	VulkanState::device.resetFences(_imageReady);
}
